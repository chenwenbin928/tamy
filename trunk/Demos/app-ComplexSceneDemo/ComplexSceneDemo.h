#pragma once

#include "core-AppFlow\Application.h"


///////////////////////////////////////////////////////////////////////////////

class SceneManager;
class UnconstrainedMotionController;

///////////////////////////////////////////////////////////////////////////////

class ComplexSceneDemo : public Application
{
private:
   Renderer* m_renderer;
   ResourceManager* m_resourceManager;

   SceneManager* m_sceneManager;
   UnconstrainedMotionController* m_cameraController;
   bool m_rotating;

public:
   ComplexSceneDemo();

   void initialize(Renderer& renderer, ResourceManager& resourceManager);

   void deinitialize();

   void update(float timeElapsed);
};

///////////////////////////////////////////////////////////////////////////////
