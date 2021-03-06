#include "dx9-Renderer\DX9Renderer.h"
#include "dx9-Renderer\DXErrorParser.h"
#include "dx9-Renderer\DX9RenderTarget.h"
#include "dx9-Renderer\DX9DebugPrimitivesSet.h"
#include "core-Renderer\Camera.h"
#include "core\Assert.h"
#include <string>


/////////////////////////////////////////////////////////////////////////////

#define ADD_RESOURCE_STORAGE( Class, Member )  Member = new Class( *this );  m_storages.push_back( Member );

/////////////////////////////////////////////////////////////////////////////

DX9Renderer::DX9Renderer( IDirect3D9& d3d9,
                          IDirect3DDevice9* d3Device, 
                          const DX9Settings& settings,
                          D3DPRESENT_PARAMETERS creationParams,
                          UINT frontBufferWidth,
                          UINT frontBufferHeight,
                          bool hardwareTLOn)
      : Renderer( frontBufferWidth, frontBufferHeight )
      , m_d3d9( d3d9 )
      , m_d3Device( d3Device )
      , m_settings( settings )
      , m_creationParams( creationParams )
      , m_deviceLost( false )
      , m_hardwareTLOn( hardwareTLOn )
      , m_activeDepthBuffer( NULL )
{
   m_viewport.X = 0;
   m_viewport.Y = 0;
   m_viewport.Width = frontBufferWidth;
   m_viewport.Height = frontBufferHeight;
   m_viewport.MinZ = 0.0f;
   m_viewport.MaxZ = 1.0f;

   // sample texture formats
   sampleOptimalTextureFormats();

   // create resource storages
   ADD_RESOURCE_STORAGE( EffectsStorage, m_effects );
   ADD_RESOURCE_STORAGE( FontStorage, m_fonts );
   ADD_RESOURCE_STORAGE( LineSegmentsStorage, m_lineSegments );
   ADD_RESOURCE_STORAGE( TriangleMeshesStorage, m_triMeshes );
   ADD_RESOURCE_STORAGE( PixelShadersStorage, m_pixelShaders );
   ADD_RESOURCE_STORAGE( SkeletonsStorage, m_skeletons );
   ADD_RESOURCE_STORAGE( VertexShadersStorage, m_vertexShaders );
   ADD_RESOURCE_STORAGE( TexturesStorage, m_textures );
   ADD_RESOURCE_STORAGE( RenderTargetsStorage, m_renderTargets );
   ADD_RESOURCE_STORAGE( DepthBuffersStorage, m_depthBuffers );
}

/////////////////////////////////////////////////////////////////////////////

DX9Renderer::~DX9Renderer()
{
   // release resource storages
   unsigned int count = m_storages.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_storages[i]->clear( *this );
      delete m_storages[i];
   }
   m_storages.clear();
   
   // release the rendering device
   if ( m_d3Device != NULL )
   {
      m_d3Device->Release();
      m_d3Device = NULL;
   }
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::initRenderer()
{
   // Setup our D3D Device initial states
   m_d3Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

   m_d3Device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
   m_d3Device->SetViewport(&m_viewport);

   // notify the storages about restored device context
   unsigned int count = m_storages.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_storages[i]->onDeviceRestored();
   }
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::resetViewport( unsigned int width, unsigned int height )
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

      // notify the storages about lost device context
      unsigned int count = m_storages.size();
      for ( unsigned int i = 0; i < count; ++i )
      {
         m_storages[i]->onDeviceLost();
      }

      // try recovering the device
      if ( FAILED( m_d3Device->Reset( &creationParams ) ) )
      {
         ASSERT_MSG( false, "Could not reset the graphical device" );
         return;
      }

      m_creationParams = creationParams;
   }

   // reinitialize the device
   initRenderer();
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::resizeViewport( unsigned int width, unsigned int height )
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

   // notify the storages about lost device context
   unsigned int count = m_storages.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_storages[i]->onDeviceLost();
   }

   // Reset the device
   D3DPRESENT_PARAMETERS creationParams = m_creationParams;
   if (creationParams.Windowed) {creationParams.BackBufferFormat = D3DFMT_UNKNOWN;}
   creationParams.BackBufferCount = 0;
   creationParams.BackBufferWidth = 0;
   creationParams.BackBufferHeight = 0;
   if ( FAILED( m_d3Device->Reset( &creationParams ) ) )
   {
      ASSERT_MSG( false, "Could not reset the graphical device" );
      return;
   }
   m_creationParams = creationParams;

   // notify the storages about lost device context
   count = m_storages.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_storages[i]->onDeviceRestored();
   }
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::renderingBegin()
{
   // clean the Z buffer
   m_d3Device->Clear( 0, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0 );
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::renderingEnd()
{
   if ( FAILED( m_d3Device->Present( NULL, NULL, NULL, NULL ) ) )
   {
      m_deviceLost = true;
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

void DX9Renderer::activateRenderTarget( RenderTarget* renderTarget, uint targetIdx )
{
   IDirect3DSurface9* renderTargetSurface = NULL;

   if ( renderTarget )
   {
      // rendering target is set - use it
      DX9RenderTarget* dxRenderTarget = m_renderTargets->getInstance( *renderTarget );
      if ( !dxRenderTarget )
      {
         ASSERT_MSG( false, "Could not create a render target implementation" );
         return;
      }

      IDirect3DTexture9* dxTex = dxRenderTarget->getDxTexture();
      HRESULT res = dxTex->GetSurfaceLevel( 0, &renderTargetSurface );
      if ( FAILED( res ) || !renderTargetSurface )
      {
         std::string errorMsg = translateDxError( "Could not acquire a render target surface", res );
         ASSERT_MSG( false, errorMsg.c_str() );
         return;
      }
   }
   else
   {
      // we're gonna be using the back buffer
      m_d3Device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &renderTargetSurface );
   }

   HRESULT res = m_d3Device->SetRenderTarget( targetIdx, renderTargetSurface );
   ASSERT_MSG( !FAILED( res ), "The specified surface could not be used as a render target" );
   renderTargetSurface->Release();
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::deactivateRenderTarget( uint targetIdx )
{
   // target with an index equal 0 can't be deactivated, thus the if clause checking that case
   if ( targetIdx > 0 )
   {
      m_d3Device->SetRenderTarget( targetIdx, NULL );
   }
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::cleanRenderTarget( const Color& bgColor )
{
   m_d3Device->Clear( 0, NULL, D3DCLEAR_TARGET, bgColor, 1.0f, 0 );
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::activateDepthBuffer( DepthBuffer& buffer )
{
   DX9DepthBuffer* depthBuffer = getDepthBuffer( buffer );
   if ( m_activeDepthBuffer == depthBuffer )
   {
      // we're trying to activate the same depth buffer that's already active - bail
      return;
   }

   if ( m_activeDepthBuffer )
   {
      m_activeDepthBuffer->deactivate();
   }
   m_activeDepthBuffer = depthBuffer;
   if ( m_activeDepthBuffer )
   {
      m_activeDepthBuffer->activate();
   }
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DVertexBuffer9* DX9Renderer::createVertexBuffer( UINT length, DWORD usageFlags, DWORD fvf, D3DPOOL memoryPool ) const
{
   ASSERT_MSG( length > 0, "Invalid size of a vertex buffer specified" );

   if ( !m_hardwareTLOn )
   {
      usageFlags |= D3DUSAGE_SOFTWAREPROCESSING;
   }

   length = ( length < 16 ) ? 16 : length;

   IDirect3DVertexBuffer9* vertexBuffer = NULL;
   HRESULT res = m_d3Device->CreateVertexBuffer( length, usageFlags, fvf, memoryPool, &vertexBuffer, NULL );

   if (FAILED(res))
   {
      std::string errorMsg = translateDxError( "Cannot create a vertex buffer", res );
      ASSERT_MSG( false, errorMsg.c_str() );
      vertexBuffer = NULL;
   }

   return vertexBuffer;
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DIndexBuffer9* DX9Renderer::createIndexBuffer( UINT length,  DWORD usageFlags, D3DFORMAT format, D3DPOOL memoryPool ) const
{
   if ( !m_hardwareTLOn ) 
   {
      usageFlags |= D3DUSAGE_SOFTWAREPROCESSING;
   }

   IDirect3DIndexBuffer9* indexBuffer = NULL;
   HRESULT res = m_d3Device->CreateIndexBuffer (length, usageFlags, format, memoryPool, &indexBuffer, NULL );

   if (  FAILED(res) )
   {
      ASSERT_MSG( false, "Cannot create an index buffer" );
      indexBuffer = NULL;
   }

   return indexBuffer;
}

/////////////////////////////////////////////////////////////////////////////

const D3DFORMAT g_ColorTextureFormats[] = { D3DFMT_DXT5, D3DFMT_DXT3, D3DFMT_X8R8G8B8, D3DFMT_DXT1, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_UNKNOWN };
const D3DFORMAT g_DepthTextureFormats[] = { D3DFMT_R32F, D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN };
const D3DFORMAT g_LuminanceTextureFormats[] = { D3DFMT_G32R32F, D3DFMT_G16R16F, D3DFMT_UNKNOWN };
const D3DFORMAT g_HDRTextureFormats[] = { D3DFMT_A32B32G32R32F, D3DFMT_A16B16G16R16F, D3DFMT_UNKNOWN };
const D3DFORMAT g_DataTextureFormats[] = { D3DFMT_A8R8G8B8, D3DFMT_UNKNOWN };
const D3DFORMAT g_32BitFPTextureFormats[] = { D3DFMT_R32F, D3DFMT_UNKNOWN };
const D3DFORMAT g_UncompressedColorTextureFormats[] = { D3DFMT_X8R8G8B8, D3DFMT_X1R5G5B5, D3DFMT_R5G6B5, D3DFMT_UNKNOWN };
const D3DFORMAT g_CompressedColorTextureFormats[] = { D3DFMT_DXT5, D3DFMT_DXT3, D3DFMT_DXT1, D3DFMT_UNKNOWN };
const D3DFORMAT g_DepthStencilTextureFormats[] = { D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D15S1, D3DFMT_UNKNOWN };
const D3DFORMAT g_PureDepthBufferTextureFormats[] = { D3DFMT_D32, D3DFMT_D16, D3DFMT_UNKNOWN };

const D3DFORMAT* g_textureFormats[] =
{
   g_ColorTextureFormats,              // TU_COLOR
   g_DepthTextureFormats,              // TU_DEPTH
   g_LuminanceTextureFormats,          // TU_LUMINANCE 
   g_HDRTextureFormats,                // TU_HDR
   g_DataTextureFormats,               // TU_DATA_TEXTURE
   g_32BitFPTextureFormats,            // TU_32BIT_FP
   g_UncompressedColorTextureFormats,  // TU_COLOR_UNCOMPRESSED
   g_CompressedColorTextureFormats,    // TU_COLOR_COMPRESSED
   g_DepthStencilTextureFormats,       // TU_DEPTH_STENCIL
   g_PureDepthBufferTextureFormats,    // TU_PURE_DEPTH_BUFFER

};

void DX9Renderer::sampleOptimalTextureFormats()
{
   for ( UINT usage = 0; usage < TU_MAX; ++usage )
   {
      const D3DFORMAT* formats = g_textureFormats[ usage ];

      UINT i = 0;
      D3DFORMAT bestFormat = D3DFMT_UNKNOWN;
      for ( UINT i = 0; formats[i] != D3DFMT_UNKNOWN; ++i )
      {
         HRESULT res = m_d3d9.CheckDeviceFormat( m_settings.adapterOrdinal, 
            m_settings.deviceType, 
            m_settings.displayMode.Format,
            0, D3DRTYPE_TEXTURE, 
            formats[i] );

         if ( SUCCEEDED( res ) )
         {
            bestFormat = formats[i];
            break;
         }
      }

      ASSERT_MSG( bestFormat != D3DFMT_UNKNOWN, "No dedicated texture format found for this usage." );
      m_bestTextureFormats[usage] = bestFormat;
   }
}

/////////////////////////////////////////////////////////////////////////////

ID3DXEffect* DX9Renderer::getEffect( EffectShader& shader )
{
   return m_effects->getInstance( shader );
}

/////////////////////////////////////////////////////////////////////////////

ID3DXFont* DX9Renderer::getFont( Font& font )
{
   return m_fonts->getInstance( font );
}

/////////////////////////////////////////////////////////////////////////////

DX9LineSegments* DX9Renderer::getLineSegments( LineSegments& segments )
{
   return m_lineSegments->getInstance( segments );
}

/////////////////////////////////////////////////////////////////////////////

DX9TriangleMesh* DX9Renderer::getTriangleMesh( TriangleMesh& mesh )
{
   return m_triMeshes->getInstance( mesh );
}

/////////////////////////////////////////////////////////////////////////////

DX9PixelShader* DX9Renderer::getPixelShader( PixelShader& shader )
{
   return m_pixelShaders->getInstance( shader );
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DVertexBuffer9* DX9Renderer::getSkeletonVertexBuffer( Skeleton& skeleton )
{
   return m_skeletons->getInstance( skeleton );
}

/////////////////////////////////////////////////////////////////////////////

DX9VertexShader* DX9Renderer::getVertexShader( VertexShader& shader )
{
   return m_vertexShaders->getInstance( shader );
}

/////////////////////////////////////////////////////////////////////////////

IDirect3DTexture9* DX9Renderer::getTexture( Texture& texture )
{
   return m_textures->getInstance( texture );
}

/////////////////////////////////////////////////////////////////////////////

DX9RenderTarget* DX9Renderer::getRenderTarget( RenderTarget& renderTarget )
{
   return m_renderTargets->getInstance( renderTarget );
}

/////////////////////////////////////////////////////////////////////////////

DX9DepthBuffer* DX9Renderer::getDepthBuffer( DepthBuffer& deepthBuffer )
{
   return m_depthBuffers->getInstance( deepthBuffer );
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::beginScene()
{
   // begin scene rendering
   m_d3Device->BeginScene();

   // reset the rendering state to its most basic form
   m_d3Device->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

/////////////////////////////////////////////////////////////////////////////

void DX9Renderer::endScene()
{
   // finish scene rendering
   m_d3Device->EndScene();
}

/////////////////////////////////////////////////////////////////////////////
