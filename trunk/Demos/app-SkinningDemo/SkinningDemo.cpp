#include "SkinningDemo.h"
#include <tchar.h>
#include "core\WindowBuilder.h"
#include "impl-DirectX\D3DRenderer.h"
#include "impl-DirectX\D3DInitializer.h"
#include "ext-MotionControllers\UnconstrainedMotionController.h"
#include "core\Timer.h"
#include <cassert>
#include "core-ResourceManagement\BasicSceneManager.h"
#include "core-Renderer\GraphicalEntityInstantiator.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Light.h"
#include "impl-DirectX\D3DResourceManager.h"
#include "core-Renderer\GraphicalEntity.h"
#include "core-Renderer\Skeleton.h"



///////////////////////////////////////////////////////////////////////////////

#define IS_PRESSED(keyboardBuffer, keyCode) (keyBuffer[keyCode] & 0xF0)

///////////////////////////////////////////////////////////////////////////////

SkinningDemo::SkinningDemo(HINSTANCE hInstance)
      : m_hInstance(hInstance),
      m_timer(new CTimer()),
      m_lastFrameRate(0),
      m_sceneManager(new BasicSceneManager()),
      m_cameraController(NULL),
      m_animationController(NULL)
{
   CWindowBuilder winBuilder;

   WindowParams params;
   strcpy_s(params.windowTitle, "Skinning Demo");
   strcpy_s(params.windowClassName, "SkinningDemoClass");
   params.ptrMsgProc = this;

   m_hWnd = winBuilder.createWindowedModeWindow(m_hInstance, params);
   assert(m_hWnd != NULL);

   m_resourceManager = new D3DResourceManager("..\\Data", "..\\Data", m_hWnd, true);
   m_renderer = &(m_resourceManager->getRendererInstance());
}

///////////////////////////////////////////////////////////////////////////////

SkinningDemo::~SkinningDemo()
{
   delete m_animationController;
   m_animationController = NULL;

   delete m_cameraController;
   m_cameraController = NULL;

   delete m_sceneManager;
   m_sceneManager = NULL;

   delete m_cameraController;
   m_cameraController = NULL;

   m_renderer = NULL;

   delete m_resourceManager;
   m_resourceManager = NULL;

   delete m_timer;
   m_timer = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SkinningDemo::run(int nCmdShow)
{
   MSG msg;
   ShowWindow(m_hWnd, nCmdShow);

   GraphicalEntityLoader& loader =  m_resourceManager->getLoaderForFile("US Ranger.x");
   AbstractGraphicalEntity& ent = m_resourceManager->loadGraphicalEntity("US Ranger.x", loader);
   GraphicalEntityInstantiator* entInstance = new GraphicalEntityInstantiator("ranger01");
   entInstance->attachEntity(ent);
   m_sceneManager->addNode(entInstance);

   m_animationController = ent.instantiateSkeleton(*entInstance);
   m_animationController->activateAnimation("");


   Light* light = m_resourceManager->createLight("light");
   light->setType(Light::LT_DIRECTIONAL);
   light->setDiffuseColor(Color(1, 1, 1, 0));
   light->setLookVec(D3DXVECTOR3(0, 0, -1));
   m_sceneManager->addNode(light);

   Camera* camera = m_resourceManager->createCamera("camera");

   D3DXMATRIX rotMtx;
   D3DXMatrixRotationYawPitchRoll(&rotMtx, D3DXToRadian(180), 0, 0);
   D3DXMatrixTranslation(&(camera->accessLocalMtx()), 0, 10, 50);
   D3DXMatrixMultiply(&(camera->accessLocalMtx()), &rotMtx, &(camera->accessLocalMtx()));
   m_sceneManager->addNode(camera);
   m_renderer->setActiveCamera(*camera);
   m_cameraController = new UnconstrainedMotionController(*camera);


   while (1) 
   {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
      {
         if (msg.message == WM_QUIT) break;
         TranslateMessage(&msg);
         DispatchMessage (&msg);
      } 
      else
      {
         advanceGameState();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void SkinningDemo::advanceGameState()
{
   m_timer->tick();
   float timeElapsed = m_timer->getTimeElapsed();

   static TCHAR titleBuffer[255];  
   if (m_lastFrameRate != m_timer->getFrameRate())
   {
      m_lastFrameRate = m_timer->getFrameRate();
      _stprintf_s(titleBuffer, _T("Skinning Demo : %ld FPS"), m_lastFrameRate);
      SetWindowText(m_hWnd, titleBuffer);
   }

   processInput(timeElapsed);
   m_animationController->update(timeElapsed);
   m_renderer->render(m_sceneManager->getRootNode());
}

///////////////////////////////////////////////////////////////////////////////

void SkinningDemo::processInput(float timeElapsed)
{
   UCHAR keyBuffer[256];
   if (!GetKeyboardState(keyBuffer)) return;

   float movementSpeed = 40;
   float rotationSpeed = 180;


   // process the keys
   if (IS_PRESSED(keyBuffer, VK_ESCAPE))     {PostQuitMessage(0);}
   if (IS_PRESSED(keyBuffer, VK_UP))     {m_cameraController->move(m_cameraController->getLookVec()   * movementSpeed * timeElapsed);}
   if (IS_PRESSED(keyBuffer, VK_DOWN))   {m_cameraController->move(-m_cameraController->getLookVec()  * movementSpeed * timeElapsed);}
   if (IS_PRESSED(keyBuffer, VK_LEFT))   {m_cameraController->move(-m_cameraController->getRightVec() * movementSpeed * timeElapsed);}
   if (IS_PRESSED(keyBuffer, VK_RIGHT))  {m_cameraController->move(m_cameraController->getRightVec()  * movementSpeed * timeElapsed);}

   // process the mouse
   if (GetCapture() == m_hWnd)
   {
      POINT cursorPos;
      GetCursorPos(&cursorPos);
      float rotX = (float)(cursorPos.x - m_oldCursorPos.x) / 3.0f;
      float rotY = (float)(cursorPos.y - m_oldCursorPos.y) / 3.0f;
      SetCursorPos(m_oldCursorPos.x, m_oldCursorPos.y);

      m_cameraController->rotate(rotY * rotationSpeed * timeElapsed, rotX * rotationSpeed * timeElapsed, 0);
   }
}

///////////////////////////////////////////////////////////////////////////////

LRESULT SkinningDemo::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_SIZE:
      {
         UINT screenWidth  = LOWORD( lParam );
         UINT screenHeight = HIWORD( lParam );

         if (m_renderer)
         {
            m_renderer->resizeViewport(screenWidth, screenHeight);
         }

         break;
      }

      case WM_LBUTTONDOWN:
      {
         SetCapture(m_hWnd);
         SetCursor(NULL);
         GetCursorPos(&m_oldCursorPos);
         break;
      }

      case WM_LBUTTONUP:
      {
        ReleaseCapture( );
        SetCursor(NULL);
        break;
      }

      case WM_DESTROY:
      {
         PostQuitMessage(0);
         break;
      }

      default:
      {
			return DefWindowProc(hWnd, message, wParam, lParam);
      }
   }
   return 0;
}

///////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR    lpCmdLine,
                   int       nCmdShow)
{
	SkinningDemo app(hInstance);
   app.run(nCmdShow);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
