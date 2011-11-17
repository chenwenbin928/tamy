/// @file   TamyEditor\SceneEditor.h
/// @brief  Simple editor for opening a new scene in the editor
#pragma once

#include "ResourceEditor.h"


///////////////////////////////////////////////////////////////////////////////

class Model;
class TimeControllerTrack;
class QScrollArea;
class Entity;
class SceneTreeViewer;
class SelectionManager;
class SceneObjectsManipulator;
enum NodeTransformControlMode;
class TamySceneWidget;

///////////////////////////////////////////////////////////////////////////////

class SceneEditor : public ResourceEditor
{
   Q_OBJECT  

private:
   Model&                                    m_scene;

   // editor related stuff
   TimeControllerTrack*                      m_sceneTrack;
   QIcon                                     m_runSceneIcon;
   QIcon                                     m_stopSceneIcon;
   QAction*                                  m_actionRun;
   bool                                      m_playing;
   TamySceneWidget*                          m_sceneWidget;
   SceneTreeViewer*                          m_sceneTreeViewer;
   SelectionManager*                         m_selectionManager;

   // objects manipulation
   SceneObjectsManipulator*                  m_sceneObjectsManipulator;
   NodeTransformControlMode                  m_objectsManipulationMode;

public:
   /**
    * Constructor.
    *
    * @param scene      scene to be edited
    */
   SceneEditor( Model& scene );
   ~SceneEditor();

   /**
    * Returns the currently set objects manipulation mode.
    */
   inline NodeTransformControlMode getObjectsManipulationMode() const { return m_objectsManipulationMode; }

   /**
    * Returns the SelectionManager instance used by the editor.
    */
   inline SelectionManager& getSelectionMgr() const { return *m_selectionManager; }

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

private:
   void runScene();
   void stopScene();

public slots:
   void saveScene();
   void toggleSceneExecution();
   void entitySelected( Entity& entity );
   void selectionCleaned();
   void setNodeTranslateMode();
   void setNodeRotateMode();
   void toggleDebugMode();
};

///////////////////////////////////////////////////////////////////////////////
