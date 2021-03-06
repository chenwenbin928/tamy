#include "TamySceneWidget.h"
#include "core.h"
#include "core-AppFlow.h"
#include "core-MVC.h"
#include "core-Renderer.h"
#include "dx9-Renderer.h"
#include "SelectedEntityRepresentation.h"
#include "SelectionRenderingPass.h"
#include <stdexcept>
#include <QEvent.h>
#include "SceneRendererInputController.h"
#include "QueryRenderingPass.h"
#include "EditorDebugRenderer.h"
#include "DebugEntitiesManager.h"
#include "Gizmo.h"


///////////////////////////////////////////////////////////////////////////////

IDirect3D9* TamySceneWidget::s_d3d9 = NULL;

///////////////////////////////////////////////////////////////////////////////

TamySceneWidget::TamySceneWidget( QWidget* parent, Qt::WindowFlags f, const FilePath& rendererPipelineName, TimeController& timeController )
   : QWidget( parent, f )
   , m_rendererPipelineName( rendererPipelineName )
   , m_localTimeController( NULL )
   , m_renderer( NULL )
   , m_camera( NULL )
   , m_scene( NULL )
   , m_renderingMech( NULL )
   , m_selectionRenderer( new SelectionRenderingPass() )
   , m_queryRenderer( new QueryRenderingPass() )
   , m_debugRenderer( new EditorDebugRenderer() )
   , m_debugEntitiesManager( new DebugEntitiesManager( *m_debugRenderer ) )
   , m_resMgr( NULL )
   , m_gizmoMode( Gizmo::GM_TRANSLATION )
   , m_gizmo( NULL )
{
   m_hWnd = static_cast< HWND >( winId() );
   memset( m_keyBuffer, 0, sizeof( unsigned char ) * 256 );

   setFocusPolicy( Qt::ClickFocus );

   if (s_d3d9 == NULL)
   {
      s_d3d9 = Direct3DCreate9( D3D_SDK_VERSION );
      if ( s_d3d9 == NULL )
      {
         ASSERT_MSG( false, "Cannot initialize DirectX library" );
         return;
      }
   }

   // acquire the application's resources manager instance
   m_resMgr = &ResourcesManager::getInstance();

   // create a renderer
   SimpleTamyConfigurator configurator;
   RenderingDevice* device = configurator.selectRenderingDevice( *s_d3d9, 800, 600, false );
   DX9Initializer initializer(*s_d3d9);
   m_renderer = initializer.createDisplay( DX9Settings(*device), m_hWnd );
   delete device;

   // create a rendering mechanism
   m_renderingMech = new CompositeRenderingMechanism();
   m_renderer->setMechanism( m_renderingMech );

   // create a camera
   m_camera = &m_renderer->getActiveCamera();
   m_camera->accessLocalMtx().setTranslation( Vector( 0, 5, 0 ) );

   // attach the dedicated  query renderer to the the debug scene
   m_debugRenderer->attachSceneView( *m_queryRenderer );

   // create and setup the time controller
   m_localTimeController = new TimeController( timeController );
   {
      m_localTimeController->add("rendering");
      m_localTimeController->get("rendering").add( *m_renderer );

      m_localTimeController->add("debugRendering");
      m_localTimeController->get("debugRendering").add( *m_debugEntitiesManager );

      m_localTimeController->add("input");
      m_localTimeController->get("input").add( *this );

      m_gizmoTrack = &m_localTimeController->add("gizmo");

      m_inputHandlerTrack = &m_localTimeController->add( "inputHandler" );
   }

   // initialize the widget
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

TamySceneWidget::~TamySceneWidget()
{
   deinitialize();
   
   m_scene = NULL;

   m_inputHandlerTrack = NULL;
   delete m_localTimeController;
   m_localTimeController = NULL;

   m_camera = NULL;

   // delete custom rendering passes
   {
      delete m_debugEntitiesManager;
      m_debugEntitiesManager = NULL;

      delete m_selectionRenderer;
      m_selectionRenderer = NULL;

      m_debugRenderer->detachSceneView( *m_queryRenderer );

      delete m_queryRenderer;
      m_queryRenderer = NULL;

      delete m_debugRenderer;
      m_debugRenderer = NULL;
   }

   m_renderingMech = NULL;
   m_renderer->setMechanism( NULL );

   delete m_renderer; m_renderer = NULL;

   m_resMgr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setScene( Model& scene )
{
   deinitialize();
   
   m_scene = &scene;

   initialize();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::clearScene()
{
   deinitialize();
   m_scene = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setRenderingPipeline( const FilePath& pipeline )
{
   deinitialize();
   m_rendererPipelineName = pipeline;
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::deinitialize()
{
   // delete the gizmo
   if ( m_gizmo )
   {
      m_gizmoTrack->remove( *m_gizmo );
      delete m_gizmo;
      m_gizmo = NULL;
   }

   // detach the views
   if ( m_scene )
   {
      m_scene->detach( *m_debugEntitiesManager );
      m_scene->detach( *m_queryRenderer );
   }

   // reset the rendering mechanism
   if ( m_renderingMech )
   {
      // don't release the selection renderer - we're managing that one on our own
      m_renderingMech->reset();
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::initialize()
{
   // initialize the rendering pipeline
   RenderingPipeline* pipeline = NULL;
   RenderingPipelineMechanism* sceneRenderer = NULL;

   try
   {
      pipeline = m_resMgr->create<RenderingPipeline>( m_rendererPipelineName );
      sceneRenderer = new RenderingPipelineMechanism( pipeline );
      m_renderingMech->add( "sceneRenderer", sceneRenderer );
      m_renderingMech->add( "debugRenderer", m_debugRenderer, false );
      m_renderingMech->add( "selectionRenderer", m_selectionRenderer, false );
      m_renderingMech->add( "queryRenderer", m_queryRenderer, false );
   }
   catch ( std::exception& ex)
   {
      ASSERT_MSG( false, ( std::string( "RenderPipeline initialization error: ") + ex.what() ).c_str() );
   }

   // setup the debug scene
   if ( sceneRenderer && m_scene )
   {
      sceneRenderer->setScene( m_scene );
   }

   // attach the views
   if ( m_scene )
   {
      m_scene->attach( *m_debugEntitiesManager );
      m_scene->attach( *m_queryRenderer );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setInputController( SceneRendererInputController* controller )
{
   m_inputHandlerTrack->reset();

   // create an input controller
   controller->initialize( *this );
  m_inputHandlerTrack->add( *controller );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::moveEvent( QMoveEvent* event )
{
   WINDOWINFO info;
   GetWindowInfo( m_hWnd, &info );

   m_renderer->resizeViewport( info.rcClient.right - info.rcClient.left, 
      info.rcClient.bottom - info.rcClient.top, 
      info.rcClient.left, 
      info.rcClient.top,
      info.rcClient.right, 
      info.rcClient.bottom );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::resizeEvent( QResizeEvent* event )
{
   WINDOWINFO info;
   GetWindowInfo( m_hWnd, &info );

   m_renderer->resizeViewport( info.rcClient.right - info.rcClient.left, 
      info.rcClient.bottom - info.rcClient.top, 
      info.rcClient.left, 
      info.rcClient.top,
      info.rcClient.right, 
      info.rcClient.bottom );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::keyPressEvent( QKeyEvent* event )
{
   setKey( m_keyBuffer, toDXKey( event->key() ), true );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::keyReleaseEvent(QKeyEvent* event )
{
   setKey( m_keyBuffer, toDXKey( event->key() ), false );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::mousePressEvent( QMouseEvent* event )
{
   switch( event->button() )
   {
   case Qt::LeftButton:
      {
         setKey( m_keyBuffer, VK_LBUTTON, true );
         break;
      }

   case Qt::RightButton:
      {
         setKey( m_keyBuffer, VK_RBUTTON, true );
         break;
      }

   case Qt::MidButton:  // fall through
   case Qt::NoButton:   // fall through
   default:
      {
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::mouseReleaseEvent( QMouseEvent* event )
{
   switch( event->button() )
   {
   case Qt::LeftButton:
      {
         setKey( m_keyBuffer, VK_LBUTTON, false );
         break;
      }

   case Qt::RightButton:
      {
         setKey( m_keyBuffer, VK_RBUTTON, false );
         break;
      }

   case Qt::MidButton:  // fall through
   case Qt::NoButton:   // fall through
   default:
      {
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::showEvent( QShowEvent* event )
{
   // reset the keys buffer
   memset( m_keyBuffer, 0, sizeof(unsigned char) * 256 );

   // pass the event higher
   __super::showEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::hideEvent( QHideEvent* event )
{
   // reset the keys buffer
   memset( m_keyBuffer, 0, sizeof(unsigned char) * 256 );

   // pass the event higher
   __super::hideEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::focusOutEvent( QFocusEvent* event )
{
   // reset the keys buffer
   memset( m_keyBuffer, 0, sizeof(unsigned char) * 256 );

   // pass the event higher
   __super::focusOutEvent( event );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onRelativeMouseMovement()
{
   this->cursor().setShape( Qt::BlankCursor );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onAbsoluteMouseMovement()
{
   this->cursor().setShape( Qt::ArrowCursor );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::checkUserInput( unsigned char* keyBuffer, Point& mousePos )
{
   memcpy_s( keyBuffer, sizeof(unsigned char) * 256, m_keyBuffer, sizeof(unsigned char) * 256 );

   QPoint cursorPos = this->cursor().pos();
   cursorPos = mapFromGlobal( cursorPos );
   mousePos.x = cursorPos.x();
   mousePos.y = cursorPos.y();
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setMousePos( const Point& pos )
{
   QPoint cursorPos( pos.x, pos.y );
   cursorPos = mapToGlobal( cursorPos );
   this->cursor().setPos( cursorPos );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::localToViewport( const Point& screenPt, Vector& viewportPt ) const
{
   float w = (float)width();
   float h = (float)height();
   float x = (float)( ( screenPt.x < (int)w ) ? screenPt.x : w );
   float y = (float)( ( screenPt.y < (int)h ) ? screenPt.y : h );

   if ( x < 0 ) x = 0;
   if ( y < 0 ) y = 0;

   viewportPt[0] = ( x / ( w * 0.5f ) ) - 1;
   viewportPt[1] = 1 - ( y / ( h * 0.5f ) );
   viewportPt[2] = 0.0f;
   viewportPt[3] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////

unsigned char TamySceneWidget::toDXKey( int qtKeyCode ) const
{
   switch( qtKeyCode )
   {
   case Qt::Key_Escape:    return VK_ESCAPE;
   case Qt::Key_Tab:       return VK_TAB;
   case Qt::Key_Backspace: return VK_CLEAR;
   case Qt::Key_Return:    return VK_RETURN;
   case Qt::Key_Enter:     return VK_TAB;
   case Qt::Key_Insert:    return VK_INSERT;
   case Qt::Key_Delete:    return VK_DELETE;
   case Qt::Key_Pause:     return VK_PAUSE;
   case Qt::Key_Print:     return VK_PRINT;
   case Qt::Key_Home:      return VK_HOME;
   case Qt::Key_End:       return VK_END;
   case Qt::Key_Left:      return VK_LEFT;
   case Qt::Key_Up:        return VK_UP;
   case Qt::Key_Right:     return VK_RIGHT;
   case Qt::Key_Down:      return VK_DOWN;
   case Qt::Key_PageUp:    return VK_PRIOR;
   case Qt::Key_PageDown:  return VK_NEXT;
   case Qt::Key_Shift:     return VK_SHIFT;
   case Qt::Key_Control:   return VK_CONTROL;
   case Qt::Key_Clear:     return VK_CLEAR;
   case Qt::Key_F1:        return VK_F1;
   case Qt::Key_F2:        return VK_F2;
   case Qt::Key_F3:        return VK_F3;
   case Qt::Key_F4:        return VK_F4;
   case Qt::Key_F5:        return VK_F5;
   case Qt::Key_F6:        return VK_F6;
   case Qt::Key_F7:        return VK_F7;
   case Qt::Key_F8:        return VK_F8;
   case Qt::Key_F9:        return VK_F9;
   case Qt::Key_F10:       return VK_F10;
   case Qt::Key_F11:       return VK_F11;
   case Qt::Key_F12:       return VK_F12;
   case Qt::Key_F13:       return VK_F13;
   case Qt::Key_F14:       return VK_F14;
   case Qt::Key_F15:       return VK_F15;
   case Qt::Key_F16:       return VK_F16;
   case Qt::Key_F17:       return VK_F17;
   case Qt::Key_F18:       return VK_F18;
   case Qt::Key_F19:       return VK_F19;
   case Qt::Key_F20:       return VK_F20;
   case Qt::Key_F21:       return VK_F21;
   case Qt::Key_F22:       return VK_F22;
   case Qt::Key_F23:       return VK_F23;
   case Qt::Key_F24:       return VK_F24;
   case Qt::Key_Space:     return VK_SPACE;
   case Qt::Key_Exclam:    return '!';
   case Qt::Key_QuoteDbl:  return '"';
   case Qt::Key_NumberSign:return '#';
   case Qt::Key_Dollar:    return '$';
   case Qt::Key_Percent:   return '%';
   case Qt::Key_Ampersand: return '&';
   case Qt::Key_Apostrophe:return '\'';
   case Qt::Key_ParenLeft: return '(';
   case Qt::Key_ParenRight:return ')';   
   case Qt::Key_Asterisk:  return '*';
   case Qt::Key_Plus:      return '+';
   case Qt::Key_Comma:     return ',';
   case Qt::Key_Minus:     return '-';
   case Qt::Key_Period:    return '.';
   case Qt::Key_Slash:     return '/';
   case Qt::Key_0:         return '0';
   case Qt::Key_1:         return '1';
   case Qt::Key_2:         return '2';
   case Qt::Key_3:         return '3';
   case Qt::Key_4:         return '4';
   case Qt::Key_5:         return '5';
   case Qt::Key_6:         return '6';
   case Qt::Key_7:         return '7';
   case Qt::Key_8:         return '8';
   case Qt::Key_9:         return '9';
   case Qt::Key_Colon:     return ':';
   case Qt::Key_Semicolon: return ';';
   case Qt::Key_Less:      return '<';
   case Qt::Key_Equal:     return '=';
   case Qt::Key_Greater:   return '>';
   case Qt::Key_Question:  return '?';
   case Qt::Key_At:        return '@';
   case Qt::Key_A:         return 'A';
   case Qt::Key_B:         return 'B';
   case Qt::Key_C:         return 'C';
   case Qt::Key_D:         return 'D';
   case Qt::Key_E:         return 'E';
   case Qt::Key_F:         return 'F';
   case Qt::Key_G:         return 'G';
   case Qt::Key_H:         return 'H';
   case Qt::Key_I:         return 'I';
   case Qt::Key_J:         return 'J';
   case Qt::Key_K:         return 'K';
   case Qt::Key_L:         return 'L';
   case Qt::Key_M:         return 'M';
   case Qt::Key_N:         return 'N';
   case Qt::Key_O:         return 'O';
   case Qt::Key_P:         return 'P';
   case Qt::Key_Q:         return 'Q';
   case Qt::Key_R:         return 'R';
   case Qt::Key_S:         return 'S';
   case Qt::Key_T:         return 'T';
   case Qt::Key_U:         return 'U';
   case Qt::Key_V:         return 'V';
   case Qt::Key_W:         return 'W';
   case Qt::Key_X:         return 'X';
   case Qt::Key_Y:         return 'Y';
   case Qt::Key_Z:         return 'Z';
   case Qt::Key_BracketLeft:  return '[';
   case Qt::Key_Backslash:    return '\\';
   case Qt::Key_BracketRight: return ']';
   case Qt::Key_Underscore:   return '_';
   case Qt::Key_BraceLeft:    return '{';
   case Qt::Key_Bar:          return '|';
   case Qt::Key_BraceRight:   return '}';
   case Qt::Key_AsciiTilde:   return '~';
   default:                return 0;
   };
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onEntitySelected( Entity& entity )
{
   m_selectionRenderer->add( entity );

   // create a manipulator gizmo
   {
      // delete the old one
      if ( m_gizmo )
      {
         m_gizmoTrack->remove( *m_gizmo );
         delete m_gizmo;
         m_gizmo = NULL;
      }

      // create a new gizmo
      if ( entity.isA< SpatialEntity >() )
      {
         m_gizmo = new Gizmo( static_cast< SpatialEntity& >( entity ), m_gizmoMode, *m_camera );
         m_gizmoTrack->add( *m_gizmo );
         m_debugRenderer->add( *m_gizmo );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::onEntityDeselected( Entity& entity )
{
   m_selectionRenderer->remove( entity );

   // remove the manipulator gizmo
   if ( m_gizmo )
   {
      m_gizmoTrack->remove( *m_gizmo );
      m_debugRenderer->remove( *m_gizmo );

      delete m_gizmo;
      m_gizmo = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::queryScene( SceneQuery& query ) const
{
   m_queryRenderer->query( query );
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setGizmoTranslationMode()
{
   m_gizmoMode = Gizmo::GM_TRANSLATION;

   if ( m_gizmo )
   {
      m_gizmo->setMode( m_gizmoMode );
   }
}

///////////////////////////////////////////////////////////////////////////////

void TamySceneWidget::setGizmoRotationMode()
{
   m_gizmoMode = Gizmo::GM_ROTATION;
   if ( m_gizmo )
   {
      m_gizmo->setMode( m_gizmoMode );
   }
}

///////////////////////////////////////////////////////////////////////////////
