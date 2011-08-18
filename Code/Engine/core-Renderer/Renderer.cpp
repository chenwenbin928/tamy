#include "core-Renderer\Renderer.h"
#include "core-Renderer\RenderingMechanism.h"
#include "core-Renderer\RenderTarget.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\Camera.h"
#include "core\Point.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class NullRenderingMechanism : public RenderingMechanism
   {
   public:
      void initialize( Renderer& renderer ) {}
      void deinitialize( Renderer& renderer ) {}
      void render( Renderer& renderer ) {}
   };

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

Renderer::Renderer(unsigned int viewportWidth,
                   unsigned int viewportHeight)
: m_mechanism( new NullRenderingMechanism() )
, m_viewportWidth(viewportWidth)
, m_viewportHeight(viewportHeight)
, m_leftClientArea(0)
, m_topClientArea(0)
, m_rightClientArea(m_viewportWidth)
, m_bottomClientArea(m_viewportHeight)
, m_initialState(new InitialState())
, m_renderingState(new RenderingState())
, m_deviceLostState(new DeviceLostState())
, m_currentRendererState( m_initialState ) // we always start in the initial state
, m_renderCommands( 1024 * 1024 ) // 1 MB for the commands
{
   m_defaultCamera = new Camera( "defaultCamera", *this );
}

///////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
   if ( m_mechanism )
   {
      m_mechanism->deinitialize( *this );
   }
   delete m_mechanism;
   m_mechanism = NULL;

   delete m_defaultCamera;
   m_defaultCamera = NULL;

   m_currentRendererState = NULL;

   delete m_initialState;
   m_initialState = NULL;

   delete m_renderingState;
   m_renderingState = NULL;

   delete m_deviceLostState;
   m_deviceLostState = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setMechanism( RenderingMechanism* mechanism )
{
   // deinitialize and release the old mechanism
   if ( m_mechanism )
   {
      m_mechanism->deinitialize( *this );
   }
   delete m_mechanism;

   // setup and initialize a new one
   if ( mechanism == NULL )
   {
      m_mechanism = new NullRenderingMechanism();
   }
   else
   {
      m_mechanism = mechanism;
   }
   m_mechanism->initialize( *this );
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::render()
{
   m_currentRendererState->render(*this);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::InitialState::render(Renderer& renderer)
{
   renderer.resetViewport(renderer.m_viewportWidth, renderer.m_viewportHeight);
   renderer.setRenderingState();

   // we don't want to loose the rendering frame just for rendering - 
   // invoke the rendering method once again so that the actual rendering
   // takes place also
   renderer.render();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::RenderingState::render( Renderer& renderer )
{
   if ( renderer.isGraphicsSystemReady() == false )
   {
      renderer.setDeviceLostState();
      return;
   }

   renderer.resetRenderTargetsList();
   renderer.renderingBegin();
   renderer.m_mechanism->render( renderer );
   renderer.executeCommands();
   renderer.renderingEnd();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::DeviceLostState::render(Renderer& renderer)
{
   renderer.attemptToRecoverGraphicsSystem();

   if (renderer.isGraphicsSystemReady() == true) {renderer.setInitialState();}
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setInitialState()
{
   m_currentRendererState = m_initialState;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setRenderingState()
{
   m_currentRendererState = m_renderingState;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setDeviceLostState()
{
   m_currentRendererState = m_deviceLostState;
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::resizeViewport(unsigned int width, unsigned int height,
                              unsigned int leftClientArea, unsigned int topClientArea,
                              unsigned int rightClientArea, unsigned int bottomClientArea)
{
   m_viewportWidth = width;
   m_viewportHeight = height;

   m_leftClientArea = leftClientArea;
   m_topClientArea = topClientArea;
   m_rightClientArea = rightClientArea;
   m_bottomClientArea = bottomClientArea;

   resetViewport(m_viewportWidth, m_viewportHeight);

   notify(RO_RESIZE_VIEWPORT);
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::screenToViewport(const Point& screenPt, 
                                D3DXVECTOR2& viewportPt) const
{
   float x = (float)((screenPt.x < (int)m_rightClientArea) ? screenPt.x : m_rightClientArea);
   float y = (float)((screenPt.y < (int)m_bottomClientArea) ? screenPt.y : m_bottomClientArea);

   x -= m_leftClientArea;
   y -= m_topClientArea;
   if (x < 0) x = 0;
   if (y < 0) y = 0;

   viewportPt.x = (x / (m_viewportWidth * 0.5f)) - 1;
   viewportPt.y = 1 - (y / (m_viewportHeight * 0.5f));
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::setRenderTarget( RenderTarget* renderTarget )
{
   static Color backBufferColor( 81, 176, 255 );

   activateRenderTarget( renderTarget );
   if ( !shouldRenderTargetBeCleaned( renderTarget ) )
   {
      return;
   }

   if ( renderTarget )
   {
      // when rendering to a render target, we DON'T want the z-buffer cleaned ( at least not at this point )
      cleanRenderTarget( renderTarget->getBackgroundColor() );
   }
   else
   {
      // we only want the z-buffer cleaned when we're cleaning the back buffer
      cleanRenderTarget( backBufferColor );
   }
   markRenderTargetCleaned( renderTarget );
}

///////////////////////////////////////////////////////////////////////////////

bool Renderer::shouldRenderTargetBeCleaned( RenderTarget* renderTarget ) const
{
   return m_renderTargetsList.find( renderTarget ) == m_renderTargetsList.end();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::markRenderTargetCleaned( RenderTarget* renderTarget )
{
   m_renderTargetsList.insert( renderTarget );
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::resetRenderTargetsList()
{
   m_renderTargetsList.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Renderer::executeCommands()
{
   RenderCommand* c = NULL;
   while ( c = m_renderCommands.front< RenderCommand >() )
   {
      c->execute( *this );
      c->~RenderCommand();
   }
}

///////////////////////////////////////////////////////////////////////////////
