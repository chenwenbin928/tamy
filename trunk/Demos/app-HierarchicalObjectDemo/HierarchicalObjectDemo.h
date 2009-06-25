#pragma once

#include "core-AppFlow\Application.h"


///////////////////////////////////////////////////////////////////////////////

class CompositeSceneManager;
class UnconstrainedMotionController;
class Renderer;

///////////////////////////////////////////////////////////////////////////////

class HierarchicalObjectDemo : public Application
{
private:
   ResourceManager* m_resMgr;
   Renderer* m_renderer;

   CompositeSceneManager* m_sceneManager;
   UnconstrainedMotionController* m_cameraController;

   bool m_rotating;

public:
   HierarchicalObjectDemo();

   void initialize(ResourceManager& resMgr);
   void deinitialize();

   void hibernate(ResourceManager& resMgr) {}
   void dehibernate(ResourceManager& resMgr) {}

   void update(float timeElapsed);

   void notify(const std::string& senderApp, int signalCode) {}
};

///////////////////////////////////////////////////////////////////////////////
