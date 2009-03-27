#pragma once

#include "core-AppFlow\Application.h"


///////////////////////////////////////////////////////////////////////////////

class SceneManager;
class UnconstrainedMotionController;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class AnimationDemo : public Application
{
private:
   Renderer* m_renderer;
   ResourceManager* m_resourceManager;

   SceneManager* m_sceneManager;
   UnconstrainedMotionController* m_cameraController;
   Skeleton* m_animationController;

   bool m_rotating;

public:
   AnimationDemo();

   void initialize(Renderer& renderer, ResourceManager& resourceManager);

   void deinitialize();

   void update(float timeElapsed);
};

///////////////////////////////////////////////////////////////////////////////
