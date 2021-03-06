#include "SkeletonAnimationEditor.h"
#include "tamyeditor.h"
#include <QCloseEvent>
#include <QSplitter>
#include <QGraphicsView>
#include <QListWidget>
#include <QCheckBox>
#include <QScrollArea>
#include "SkeletonAnimationKeysChart.h"
#include "SkeletonAnimationEventsChart.h"
#include "VerticalChartMarker.h"
#include "core-AI/SkeletonAnimation.h"
#include "core-AI/SkeletonAnimationController.h"
#include "core-MVC/ModelEntity.h"
#include "core-MVC/Model.h"
#include "core-AppFlow.h"
#include "core-Renderer/Skeleton.h"
#include "TamySceneWidget.h"
#include "CameraMovementController.h"
#include "ChartView.h"
#include <QSettings>
#include <QSlider>
#include "AnimChartScales.h"
#include "AnimationTimeValueGetter.h"
#include "DropFrame.h"
#include "FSNodeMimeData.h"


///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationEditor::SkeletonAnimationEditor( SkeletonAnimation& animation )
   : TIME_SCALE( 1000.0f )
   , m_animation( animation )
   , m_animationKeysChart( new SkeletonAnimationKeysChart() )
   , m_animationEventsChart( new SkeletonAnimationEventsChart() )
   , m_bonesList( NULL )
   , m_scene( NULL )
   , m_timeTrack( NULL )
   , m_animationController( NULL )
   , m_keysTimeTrackingMarker( NULL )
   , m_eventsTimeTrackingMarker( NULL )
   , m_playing( false )
{
}

///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationEditor::~SkeletonAnimationEditor()
{
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onInitialize()
{
   TamyEditor& mainEditor = TamyEditor::getInstance();

   TimeController& controller = mainEditor.getTimeController();
   char uniqueName[256];
   sprintf_s( uniqueName, "animationEditor_%p", this );
   m_timeTrack = &controller.add( uniqueName );

   ResourcesManager& resMgr = ResourcesManager::getInstance();
   QString iconsDir = resMgr.getFilesystem().getShortcut( "editorIcons" ).c_str();

   // setup ui
   m_ui.setupUi( this );

   // attach the graph viewer
   QSplitter* mainSplitter = new QSplitter( this );
   m_ui.chartsFrameLayout->addWidget( mainSplitter );
   mainSplitter->setOrientation( Qt::Horizontal );

   // left side frames
   {
      QSplitter* operationsSplitter = new QSplitter( mainSplitter );
      mainSplitter->addWidget( operationsSplitter );
      operationsSplitter->setOrientation( Qt::Vertical );

      TimeController& timeController = mainEditor.getTimeController();
      
      // create the preview frame
      QFrame* animationPreviewFrame = new QFrame( operationsSplitter );
      operationsSplitter->addWidget( animationPreviewFrame );
      QVBoxLayout* previewFrameMasterLayout = new QVBoxLayout();
      animationPreviewFrame->setLayout( previewFrameMasterLayout );
      animationPreviewFrame->resize( 200, 200 );

      {
         // create the scene renderer
         DropFrame* renderViewportFrame = new DropFrame( animationPreviewFrame, new FSNodeMimeData( m_animatedModelPaths ) );
         connect( renderViewportFrame, SIGNAL( changed() ), this, SLOT( animatedModelLoaded() ) );

         previewFrameMasterLayout->addWidget( renderViewportFrame, 1 );
         QLayout* renderViewportLayout = new QHBoxLayout();
         renderViewportFrame->setLayout( renderViewportLayout );
         renderViewportLayout->setMargin( 0 );
         renderViewportLayout->setSpacing( 0 );
         renderViewportFrame->setFrameStyle( QFrame::Panel | QFrame::Sunken );
         renderViewportFrame->setLineWidth( 2 );

         QSettings& settings = mainEditor.getSettings();
         settings.beginGroup( "SkeletonAnimationEditorRenderer" );
         std::string rendererPipelineName = settings.value( "pipeline", "" ).toString().toStdString();
         settings.endGroup();

         m_sceneWidget = new TamySceneWidget( renderViewportFrame, 0, rendererPipelineName, timeController );
         renderViewportLayout->addWidget( m_sceneWidget );
         m_sceneWidget->setInputController( new CameraMovementController() );

         // add a control panel so that we can play the scene
         QToolBar* playbackControlPanel = new QToolBar( animationPreviewFrame );
         previewFrameMasterLayout->addWidget( playbackControlPanel, 0 );

         m_runSceneIcon = QIcon( iconsDir + tr( "/play.png" ) );
         m_stopSceneIcon = QIcon( iconsDir + tr( "/stop.png" ) );

         m_actionPlay = new QAction( m_runSceneIcon, tr( "Run" ), playbackControlPanel );
         playbackControlPanel->addAction( m_actionPlay );
         connect( m_actionPlay, SIGNAL( triggered() ), this, SLOT( togglePlay() ) );
         m_actionPlay->setEnabled( false );
      }

      m_bonesList = new QListWidget( operationsSplitter );
      operationsSplitter->addWidget( m_bonesList );
      connect( m_bonesList, SIGNAL( itemClicked( QListWidgetItem* ) ), this, SLOT( onBoneSelected( QListWidgetItem* ) ) );
   }

   // right side frames
   {
      DropFrame* mainChartsFrame = new DropFrame( mainSplitter, new FSNodeMimeData( m_animationsPaths )  );
      connect( mainChartsFrame, SIGNAL( changed() ), this, SLOT( animationImported() ) );

      mainSplitter->addWidget( mainChartsFrame );

      QHBoxLayout* mainChartsFrameLayout = new QHBoxLayout( mainChartsFrame );
      mainChartsFrame->setLayout( mainChartsFrameLayout );

      // keys selection frame
      {
         QFrame* keysSelectionFrame = new QFrame( mainChartsFrame );
         mainChartsFrameLayout->addWidget( keysSelectionFrame );

         QVBoxLayout* keysSelectionLayout = new QVBoxLayout( keysSelectionFrame );
         keysSelectionFrame->setLayout( keysSelectionLayout );

         m_posKeyXCheckBox = new QCheckBox( "X", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_posKeyXCheckBox );
         connect( m_posKeyXCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onTogglePosKeyX( int ) ) );

         m_posKeyYCheckBox = new QCheckBox( "Y", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_posKeyYCheckBox );
         connect( m_posKeyYCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onTogglePosKeyY( int ) ) );

         m_posKeyZCheckBox = new QCheckBox( "Z", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_posKeyZCheckBox );
         connect( m_posKeyZCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onTogglePosKeyZ( int ) ) );

         m_orientKeyYawCheckBox = new QCheckBox( "Yaw", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_orientKeyYawCheckBox );
         connect( m_orientKeyYawCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onToggleOrientKeyYaw( int ) ) );

         m_orientKeyPitchCheckBox = new QCheckBox( "Pitch", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_orientKeyPitchCheckBox );
         connect( m_orientKeyPitchCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onToggleOrientKeyPitch( int ) ) );

         m_orientKeyRollCheckBox = new QCheckBox( "Roll", keysSelectionFrame );
         keysSelectionLayout->addWidget( m_orientKeyRollCheckBox );
         connect( m_orientKeyRollCheckBox, SIGNAL( stateChanged( int ) ), this, SLOT( onToggleOrientKeyRoll( int ) ) );
      }

      // charts section
      {
         QFrame* animationChartsFrame = new QFrame( mainChartsFrame );
         mainChartsFrameLayout->addWidget( animationChartsFrame );
         QVBoxLayout* animationChartsFrameLayout = new QVBoxLayout( animationChartsFrame );
         animationChartsFrame->setLayout( animationChartsFrameLayout );

         // splitter with the keys and events charts
         {
            QSplitter* animationChartsSplitter = new QSplitter( animationChartsFrame );
            animationChartsSplitter->setOrientation( Qt::Vertical );
            animationChartsFrameLayout->addWidget( animationChartsSplitter );

            // keys chart
            {
               m_animationKeysChartView = new ChartView( m_animationKeysChart, animationChartsSplitter );
               animationChartsSplitter->addWidget( m_animationKeysChartView );
               m_animationKeysChartView->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
            }
         }
         
         // time control slider
         {
            m_timeControlWidget = new QSlider( Qt::Horizontal, animationChartsFrame );
            animationChartsFrameLayout->addWidget( m_timeControlWidget );
            m_timeControlWidget->setRange( 0, m_animation.getAnimationLength() * TIME_SCALE );
            connect( m_timeControlWidget, SIGNAL( sliderMoved( int ) ), this, SLOT( onTimeValueChanged( int ) ) );

            // disable the slider until an animated model is loaded
            m_timeControlWidget->setEnabled( false );
         }
      }
   }

   // adjust the main splitter size
   QList< int > sizes;
   sizes.push_back( 150 );
   sizes.push_back( 1000 );
   mainSplitter->setSizes( sizes );

   // show the editor
   refresh();
   show();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onDeinitialize( bool saveProgress )
{
   TamyEditor& mainEditor = TamyEditor::getInstance();
   TimeController& controller = mainEditor.getTimeController();
   m_timeTrack->reset();
   controller.remove( m_timeTrack->getID() );
   m_timeTrack = NULL;

   m_animationController = NULL;

   m_sceneWidget->clearScene();
   delete m_scene;
   m_scene = NULL;

   delete m_animationKeysChart;
   m_animationKeysChart = NULL;

   delete m_animationEventsChart;
   m_animationEventsChart = NULL;

   m_bonesList = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::refresh()
{
   // reset the bones list
   m_bonesList->clear();

   std::vector< std::string > bonesNames;
   m_animation.collectBonesNames( bonesNames );
   for ( std::vector< std::string >::const_iterator it = bonesNames.begin(); it != bonesNames.end(); ++it )
   {
      m_bonesList->addItem( it->c_str() );
   }

   // reset the selected bones
   m_posKeyXCheckBox->setChecked( false );
   m_posKeyYCheckBox->setChecked( false );
   m_posKeyZCheckBox->setChecked( false );
   m_orientKeyYawCheckBox->setChecked( false );
   m_orientKeyPitchCheckBox->setChecked( false );
   m_orientKeyRollCheckBox->setChecked( false );

   // reset the charts
   m_animationKeysChart->setAnimation( m_animation );
   m_animationEventsChart->setAnimation( m_animation );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onBoneSelected( QListWidgetItem* item )
{
   std::string boneName = item->text().toStdString();
   m_animationKeysChart->toggleBone( boneName );
   if ( m_animationKeysChart->isBoneDisplayed( boneName ) )
   {
      item->setTextColor( QColor( 204, 170, 0 ) );
   }
   else
   {
      item->setTextColor( QColor( 0, 0, 0 ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onTogglePosKeyX( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_POS_X );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onTogglePosKeyY( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_POS_Y );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onTogglePosKeyZ( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_POS_Z );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onToggleOrientKeyYaw( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_YAW );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onToggleOrientKeyPitch( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_PITCH );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onToggleOrientKeyRoll( int state )
{
   m_animationKeysChart->toggleKeyVisibility( BAKEY_ROLL );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::togglePlay()
{
   if ( m_scene == NULL )
   {
      // still no scene - break it
      return;
   }

   if ( m_playing )
   {
      // remove the items from the time track
      m_animationController->pause( true );
      m_timeTrack->remove( *this );
      m_timeTrack->remove( *m_animationKeysChartView );

      // enable the control over the time slider 
      m_timeControlWidget->setEnabled( true );

      // update the UI
      m_actionPlay->setIcon( m_runSceneIcon );
      m_actionPlay->setText( "Run" );
   }
   else
   {
      m_animationController->pause( false );
      m_timeTrack->add( *this );
      m_timeTrack->add( *m_animationKeysChartView );

      // disable the control over the time slider 
      m_timeControlWidget->setEnabled( false );

      // update the UI
      m_actionPlay->setIcon( m_stopSceneIcon );
      m_actionPlay->setText( "Stop" );
   }
   m_playing = !m_playing;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::animatedModelLoaded()
{
   if ( !m_animatedModelPaths.empty() )
   {
      ResourcesManager& rm = ResourcesManager::getInstance();
      loadAnimatedModel( m_animatedModelPaths[ 0 ] );
   }

   m_animatedModelPaths.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::loadAnimatedModel( const std::string& modelResourcePath )
{
   // load the skeleton
   Model* modelForAnimation = ResourcesManager::getInstance().create< Model >( modelResourcePath );
   if ( !modelForAnimation )
   {
      // couldn't load the model resource
      return;
   }

   Model* newScene = new Model();
   newScene->add( new ModelEntity( *modelForAnimation, "animatedModel" ) );

   Entity* skeletonRootEntity = newScene->findFirstEntity( "Hips" );
   if ( !skeletonRootEntity )
   {
      // no root bone
      delete newScene;
      return;
   }

   // destroy the previous scene & replace it with the freshly created one
   if ( m_scene )
   {
      m_sceneWidget->clearScene();
   }
   delete m_scene;
   m_scene = newScene;

   // create a scene using the animation
   m_animationController = new SkeletonAnimationController( "animation" );
   m_animationController->setAnimationSource( m_animation );
   skeletonRootEntity->add( m_animationController );

   // start in pause
   m_animationController->pause( true );

   m_sceneWidget->setScene( *m_scene );

   // the scene should be updated all the time
   m_timeTrack->add( *m_scene );

   // enable the time control slider
   m_timeControlWidget->setEnabled( true );

   // add the time tracking widgets
   m_keysTimeTrackingMarker = new VerticalChartMarker( new AnimationTimeValueGetter( *m_animationController, ANIMATION_TIME_SCALE ) );
   m_eventsTimeTrackingMarker = new VerticalChartMarker( new AnimationTimeValueGetter( *m_animationController, ANIMATION_TIME_SCALE ) );
   m_timeTrack->add( *m_keysTimeTrackingMarker );
   m_timeTrack->add( *m_eventsTimeTrackingMarker );
   m_animationKeysChart->addItem( m_keysTimeTrackingMarker );
   m_animationEventsChart->addItem( m_eventsTimeTrackingMarker );

   // activate the play action
   m_actionPlay->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::update( float timeElapsed )
{
   if ( m_animationController )
   {
      m_timeControlWidget->setValue( m_animationController->getTrackTime() * TIME_SCALE );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::onTimeValueChanged( int newValue )
{
   if ( !m_animationController )
   {
      return;
   }

   float newTimeValue = newValue / TIME_SCALE;
   m_animationController->setTrackTime( newTimeValue );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationEditor::animationImported()
{
   ResourcesManager& rm = ResourcesManager::getInstance();

   unsigned int count = m_animationsPaths.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      FilePath path = m_animationsPaths[i];

      TResourceImporter< SkeletonAnimation >* importer = rm.createImporter< SkeletonAnimation >( path );
      if ( importer )
      {
         importer->import( m_animation );
         delete importer;

         // ok - animation imported, no need to go through the remaining paths
         break;
      }
   }

   m_animationsPaths.clear();

   refresh();
}

///////////////////////////////////////////////////////////////////////////////
