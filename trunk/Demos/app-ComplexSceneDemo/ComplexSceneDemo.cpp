#include "ComplexSceneDemo.h"
#include "impl-DirectX\D3DApplicationManager.h"
#include "core-AppFlow\ExecutionContext.h"
#include "core-Renderer\Renderer.h"
#include "core\Point.h"
#include "core-ResourceManagement\ResourceManager.h"
#include "core\CompositeSceneManager.h"
#include "core-Renderer\VisualSceneManager.h"
#include "core-Renderer\GraphicalEntityInstantiator.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Light.h"
#include "core-ResourceManagement\IWFLoader.h"
#include "core-Renderer\GraphicalEntity.h"
#include "core-Renderer\Skeleton.h"
#include "core-ResourceManagement\GraphicalEntityLoader.h"
#include "ext-MotionControllers\UnconstrainedMotionController.h"


///////////////////////////////////////////////////////////////////////////////

ComplexSceneDemo::ComplexSceneDemo()
      : Application("Demo"),
      m_resMgr(NULL),
      m_renderer(NULL),
      m_sceneManager(NULL),
      m_cameraController(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

void ComplexSceneDemo::initialize(ResourceManager& resMgr)
{
   m_resMgr = &resMgr;
   m_renderer = &(resMgr.shared<Renderer>());

   m_rotating = false;
   m_sceneManager = new CompositeSceneManager();
   VisualSceneManager* visualSceneManager = new VisualSceneManager(64, 2000);
   m_sceneManager->addSceneManager(visualSceneManager);
   m_renderer->addVisualSceneManager(*visualSceneManager);

   IWFLoader loader(*m_resMgr, *m_sceneManager);
   loader.load("..\\Data\\Colony5.iwf");

   Camera* camera = new Camera("camera");
   m_sceneManager->addNode(camera);
   m_cameraController = new UnconstrainedMotionController(*camera);
}

///////////////////////////////////////////////////////////////////////////////

void ComplexSceneDemo::deinitialize()
{
   delete m_cameraController;
   m_cameraController = NULL;

   delete m_sceneManager;
   m_sceneManager = NULL;

   m_renderer = NULL;
   m_resMgr = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ComplexSceneDemo::update(float timeElapsed)
{
   float movementSpeed = 40 * timeElapsed;
   float rotationSpeed = 180 * timeElapsed;


   // process the keys
   if (context().isKeyPressed(VK_ESCAPE))     {context().signal(*this, ON_EXIT);}
   if (context().isKeyPressed(VK_UP))     {m_cameraController->move(m_cameraController->getLookVec()   * movementSpeed);}
   if (context().isKeyPressed(VK_DOWN))   {m_cameraController->move(-m_cameraController->getLookVec()  * movementSpeed);}
   if (context().isKeyPressed(VK_LEFT))   {m_cameraController->move(-m_cameraController->getRightVec() * movementSpeed);}
   if (context().isKeyPressed(VK_RIGHT))  {m_cameraController->move(m_cameraController->getRightVec()  * movementSpeed);}

   if (context().isKeyPressed(VK_LBUTTON) && (m_rotating == false))
   {
      context().relativeMouseMovement(true);
      m_rotating = true;
   }
   else if ((context().isKeyPressed(VK_LBUTTON) == false) && m_rotating)
   {
      context().relativeMouseMovement(false);
      m_rotating = false;
   }

   // process the mouse
   if (m_rotating)
   {
      Point mouseRel = context().getMousePos();
      float rotX = (float)(mouseRel.x) / 3.0f;
      float rotY = (float)(mouseRel.y) / 3.0f;
      m_cameraController->rotate(rotY * rotationSpeed, rotX * rotationSpeed, 0);
   }

   m_renderer->render();
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR    lpCmdLine,
                   int       nCmdShow)
{
   D3DApplicationManager applicationManager("..\\Data", "..\\Data", "..\\Data",
                                            hInstance, nCmdShow, "Complex Scene Demo");
	ComplexSceneDemo app;

   applicationManager.addApplication(app);
   applicationManager.setEntryApplication(app.getName());

   while (applicationManager.step()) {}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
