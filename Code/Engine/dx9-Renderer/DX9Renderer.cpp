#include "dx9-Renderer\DX9Renderer.h"
#include <stdexcept>
#include <string>
#include <cassert>

// implementations
#include "core-Renderer\TriangleMesh.h"
#include "dx9-Renderer\DX9TriangleMesh.h"
#include "core-Renderer\LineSegments.h"
#include "dx9-Renderer\DX9LineSegments.h"
#include "core-Renderer\Texture.h"
#include "dx9-Renderer\DX9Texture.h"
#include "core-Renderer\Shader.h"
#include "dx9-Renderer\DX9Shader.h"
#include "core-Renderer\Font.h"
#include "dx9-Renderer\DX9Font.h"


/////////////////////////////////////////////////////////////////////////////

DX9Renderer::DX9Renderer(IDirect3DDevice9* d3Device, 
                         D3DPRESENT_PARAMETERS creationParams,
                         D3DCAPS9 caps,
                         UINT frontBufferWidth,
                         UINT frontBufferHeight,
                         bool hardwareTLOn,
                         D3DFORMAT optimalTextureFormat)
      : Renderer(frontBufferWidth, frontBufferHeight),
      m_d3Device(d3Device),
      m_creationParams(creationParams),
      m_caps(caps),
      m_deviceLost(false),
      m_hardwareTLOn(hardwareTLOn),
      m_maxLights(255),
      m_optimalTextureFormat(optimalTextureFormat)
{
   if (m_hardwareTLOn)
   {
      m_maxLights = m_caps.MaxActiveLights;
   }

   m_viewport.X = 0;
   m_viewport.Y = 0;
   m_viewport.Width = frontBufferWidth;
   m_viewport.Height = frontBufferHeight;
   m_viewport.MinZ = 0.0f;
   m_viewport.MaxZ = 1.0f;

   // associate implementations
   associate<TriangleMesh, DX9TriangleMesh> ();
   associate<LineSegments, DX9LineSegments> ();
   associate<Texture, DX9Texture> ();
   associate<Shader, DX9Shader> ();
   associate<Font, DX9Font> ();
}

/////////////////////////////////////////////////////////////////////////////

DX9Renderer::~DX9Renderer()
{
   if (m_d3Device != NULL)
   {
      m_d3Device->Release();
      m_d3Device = NULL;
   }
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::initRenderer()
{
   // Setup our D3D Device initial states
   m_d3Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);//D3DCULL_CCW);

   m_d3Device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
   m_d3Device->SetViewport(&m_viewport);

   Subject<DX9Renderer, DX9GraphResourceOp>::notify(GRO_CREATE_RES);
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::resetViewport(unsigned int width, unsigned int height)
{
   if ((m_d3Device) && (m_creationParams.Windowed == TRUE))
   {
      // Store new sizes
      m_creationParams.BackBufferWidth = width;
      m_creationParams.BackBufferHeight = height;

      m_viewport.X = 0;
      m_viewport.Y = 0;
      m_viewport.Width = width;
      m_viewport.Height = height;
      m_viewport.MinZ = 0.0f;
      m_viewport.MaxZ = 1.0f;

      // Reset the device
      D3DPRESENT_PARAMETERS creationParams = m_creationParams;
      if (creationParams.Windowed) {creationParams.BackBufferFormat = D3DFMT_UNKNOWN;}
      creationParams.BackBufferCount = 0;
      creationParams.BackBufferWidth = 0;
      creationParams.BackBufferHeight = 0;

      Subject<DX9Renderer, DX9GraphResourceOp>::notify(GRO_RELEASE_RES);

      if (FAILED(m_d3Device->Reset(&creationParams)))
      {
         throw std::runtime_error("Could not reset the graphical device");
      }

      m_creationParams = creationParams;
   }
   initRenderer();
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::renderingBegin()
{
   m_d3Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFF514FFF, 1.0f, 0);

   m_d3Device->BeginScene();
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::renderingEnd()
{
   m_d3Device->EndScene();
   if (FAILED(m_d3Device->Present(NULL, NULL, NULL, NULL)))
   {
      m_deviceLost = true;
      return;
   }
}

/////////////////////////////////////////////////////////////////////////////

bool DX9Renderer::isGraphicsSystemReady() const
{
   return !(m_deviceLost == true);
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::attemptToRecoverGraphicsSystem()
{
   HRESULT deviceStatus = m_d3Device->TestCooperativeLevel();

   switch(deviceStatus)
   {
   case D3D_OK:
      {
         m_deviceLost = false;
         break;
      }

   case D3DERR_DEVICENOTRESET:
      {
         HRESULT resettingResult = m_d3Device->Reset(&m_creationParams);

         if (SUCCEEDED(resettingResult))
         {
            m_deviceLost = false;
         }
         break;
      }

   default:
      break;
   }

   if (m_deviceLost == false)
   {
      initRenderer();
   }
}

/////////////////////////////////////////////////////////////////////////////

UINT DX9Renderer::getMaxLightsCount() const
{
   return m_maxLights;
}

/////////////////////////////////////////////////////////////////////////////

UINT DX9Renderer::getSimultaneouslyRenderedTexturesCount() const
{
   UINT count = m_caps.MaxSimultaneousTextures;
   if (count > m_caps.MaxTextureBlendStages) count = m_caps.MaxTextureBlendStages;

   return count;
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DVertexBuffer9* DX9Renderer::createVertexBuffer(UINT length, 
                                                        DWORD usageFlags, 
                                                        DWORD fvf, 
                                                        D3DPOOL memoryPool)
{
   assert(length > 0);

   if (!m_hardwareTLOn) usageFlags |= D3DUSAGE_SOFTWAREPROCESSING;

   length = (length < 16) ? 16 : length;

   IDirect3DVertexBuffer9* vertexBuffer = NULL;
   HRESULT res = m_d3Device->CreateVertexBuffer(length,
                                                usageFlags,
                                                fvf,
                                                memoryPool,
                                                &vertexBuffer,
                                                NULL);

   if (FAILED(res))
   {
      std::string errCode = "Cannot create a vertex buffer ";
      switch(res)
      {
      case D3DERR_INVALIDCALL:
         errCode += "- invalid parameters used";
         break;

      case D3DERR_OUTOFVIDEOMEMORY:
         errCode += "- not enough video memory left";
         break;

      case E_OUTOFMEMORY:
         errCode += "- not enough system memory left";
         break;

      default:
         errCode += "for an unknown reason";
         break;
      }
      throw std::logic_error(errCode);
   }

   return vertexBuffer;
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DIndexBuffer9* DX9Renderer::createIndexBuffer(UINT length, 
                                                      DWORD usageFlags, 
                                                      D3DFORMAT format, 
                                                      D3DPOOL memoryPool)
{
   if (!m_hardwareTLOn) usageFlags |= D3DUSAGE_SOFTWAREPROCESSING;

   IDirect3DIndexBuffer9* indexBuffer = NULL;

   HRESULT res = m_d3Device->CreateIndexBuffer(length,
      usageFlags,
      format,
      memoryPool,
      &indexBuffer,
      NULL);

   if (FAILED(res))
   {
      throw std::logic_error(
         std::string("Cannot create an index buffer"));
   }

   return indexBuffer;
}

/////////////////////////////////////////////////////////////////////////////