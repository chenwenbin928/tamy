#include "dx9-Renderer\DX9PixelShader.h"
#include "dx9-Renderer\DX9Renderer.h"
#include "core-Renderer\ShaderTexture.h"
#include "core\File.h"
#include "core\Assert.h"
#include "dx9-Renderer\DXErrorParser.h"
#include "dx9-Renderer/DX9ShaderIncludeLoader.h"


///////////////////////////////////////////////////////////////////////////////

void RCBindPixelShader::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );

   DX9PixelShader* dxShader = dxRenderer.getPixelShader( m_shader );
   if ( !dxShader )
   {
      return;
   }

   // set the shader parameters
   setParams( renderer, dxShader );
   dxShader->beginRendering();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void RCUnbindPixelShader::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   DX9PixelShader* dxShader = dxRenderer.getPixelShader( m_shader );
   if ( dxShader )
   {
      // reset the rendering state
      dxShader->endRendering();
   }

   // and reset the shader
   dxRenderer.getD3Device().SetPixelShader( NULL );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DX9PixelShader::DX9PixelShader( const DX9Renderer& renderer, const PixelShader& shader )
   : m_shader( shader )
   , m_renderer( renderer )
   , m_d3Device( &renderer.getD3Device() )
   , m_dxPixelShader( NULL )
   , m_shaderConstants( NULL )
{
   compile();
}

///////////////////////////////////////////////////////////////////////////////

DX9PixelShader::~DX9PixelShader()
{
   m_d3Device = NULL;

   if ( m_dxPixelShader != NULL )
   {
      m_dxPixelShader->Release();
      m_dxPixelShader = NULL;
   }

   if ( m_shaderConstants )
   {
      m_shaderConstants->Release();
      m_shaderConstants = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::compile()
{
   // release the old shader
   if ( m_dxPixelShader != NULL )
   {
      m_dxPixelShader->Release();
      m_dxPixelShader = NULL;
   }

   if ( m_shaderConstants )
   {
      m_shaderConstants->Release();
      m_shaderConstants = NULL;
   }

   // load the effect
   const std::string& shaderContents = m_shader.getScript();

   if ( shaderContents.empty() )
   {
      return;
   }

   const char* shaderProfile = D3DXGetPixelShaderProfile( m_d3Device );

   DWORD flags = 0;
#ifdef _DEBUG
   flags = D3DXSHADER_DEBUG;
#endif

   ID3DXBuffer* shaderBuf = NULL;
   ID3DXBuffer* errorsBuf = NULL;
   DX9ShaderIncludeLoader includesLoader;
   HRESULT res = D3DXCompileShader(
      shaderContents.c_str(), 
      shaderContents.length(),
      NULL,                            // defines
      &includesLoader,
      m_shader.getEntryFunctionName().c_str(),
      shaderProfile, 
      flags,
      &shaderBuf, 
      &errorsBuf,
      &m_shaderConstants );

   if ( FAILED(res) || shaderBuf == NULL )
   {
      if ( errorsBuf != NULL )
      {
         std::string compilationErrors = ( const char* )errorsBuf->GetBufferPointer();
         errorsBuf->Release();
         std::string errMsg = std::string( "Shader compilation error: " ) + compilationErrors;
         ASSERT_MSG( false, errMsg.c_str() );
      }
      else
      {
         std::string errMsg = translateDxError( "Error while compiling a shader", res );
         ASSERT_MSG( false, errMsg.c_str() );
      }

      return;
   }

   res = m_d3Device->CreatePixelShader( ( const DWORD* )shaderBuf->GetBufferPointer(), &m_dxPixelShader );
   shaderBuf->Release();
   if ( FAILED(res) )
   {
      std::string errMsg = translateDxError( "Error while creating a shader", res );
      ASSERT_MSG( false, errMsg.c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setBool( const char* paramName, bool val )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      m_shaderConstants->SetBool( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setInt( const char* paramName, int val )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      m_shaderConstants->SetInt( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setIntArray( const char* paramName, int* valsArr, unsigned int size )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      HRESULT res = m_shaderConstants->SetIntArray( m_d3Device, hConstant, valsArr, size );
      ASSERT( SUCCEEDED( res ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setFloat( const char* paramName, float val )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      m_shaderConstants->SetFloat( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setFloatArray( const char* paramName, float* valsArr, unsigned int size )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_shaderConstants->SetFloatArray( m_d3Device, hConstant, valsArr, size );
         ASSERT( SUCCEEDED( res ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setMtx( const char* paramName, const D3DXMATRIX& matrix )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      m_shaderConstants->SetMatrix( m_d3Device, hConstant, &matrix );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setMtxArray( const char* paramName, const D3DXMATRIX* matrices, unsigned int size )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_shaderConstants->SetMatrixArray( m_d3Device, hConstant, matrices, size );
         ASSERT( SUCCEEDED( res ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setVec4( const char* paramName, const D3DXVECTOR4& vec )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      m_shaderConstants->SetVector( m_d3Device, hConstant, &vec );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setVec4Array( const char* paramName, const D3DXVECTOR4* vecArr, unsigned int size )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      if ( hConstant != NULL )
      {
         HRESULT res = m_shaderConstants->SetVectorArray( m_d3Device, hConstant, vecArr, size );
         ASSERT_MSG( SUCCEEDED( res ), translateDxError( "setVec4Array", res ).c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::setTexture( const char* paramName, IDirect3DTexture9* texture )
{
   if ( m_shaderConstants )
   {
      D3DXHANDLE hConstant = m_shaderConstants->GetConstantByName( NULL, paramName );
      UINT samplerIdx = m_shaderConstants->GetSamplerIndex( hConstant );
      if ( samplerIdx > 16 )
      {
         // invalid sampler index
         return;
      }

      const TextureStageParams& params = m_shader.getTextureStage( samplerIdx );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_ADDRESSU, params.m_addressU );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_ADDRESSV, params.m_addressV );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_ADDRESSW, params.m_addressW );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_MINFILTER, params.m_minFilter );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_MAGFILTER, params.m_magFilter );
      m_d3Device->SetSamplerState( samplerIdx, D3DSAMP_MIPFILTER, params.m_mipFilter );

      m_d3Device->SetTexture( samplerIdx, texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::beginRendering()
{
   const PixelShaderParams& params = m_shader.getParams();
   
   // geometry
   m_d3Device->SetRenderState( D3DRS_CULLMODE, params.m_cullingMode );

   // depth buffer
   m_d3Device->SetRenderState( D3DRS_ZENABLE, params.m_useZBuffer );
   m_d3Device->SetRenderState( D3DRS_ZWRITEENABLE, params.m_writeToZBuffer );
   m_d3Device->SetRenderState( D3DRS_ZFUNC, params.m_depthTestFunc );

   // alpha tests
   m_d3Device->SetRenderState( D3DRS_ALPHATESTENABLE, params.m_useAlphaTest );
   if ( params.m_useAlphaTest )
   {
      m_d3Device->SetRenderState( D3DRS_ALPHAFUNC, params.m_alphaTestFunc );
      m_d3Device->SetRenderState( D3DRS_ALPHAREF, params.m_useAlphaTest );
   }

   // alpha blending
   m_d3Device->SetRenderState( D3DRS_ALPHABLENDENABLE, params.m_useBlending );
   if ( params.m_useBlending )
   {
      m_d3Device->SetRenderState( D3DRS_SRCBLEND, params.m_blendSourceFunc );
      m_d3Device->SetRenderState( D3DRS_DESTBLEND, params.m_blendDestFunc );

      m_d3Device->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, params.m_useSeparateAlphaBlend );
      if ( params.m_useSeparateAlphaBlend )
      {
         m_d3Device->SetRenderState( D3DRS_SRCBLENDALPHA, params.m_alphaBlendSourceFunc );
         m_d3Device->SetRenderState( D3DRS_DESTBLENDALPHA, params.m_alphaBlendDestFunc );
      }
   }

   // stencil operations
   m_d3Device->SetRenderState( D3DRS_STENCILENABLE, params.m_stencilEnable );
   if ( params.m_stencilEnable )
   {
      m_d3Device->SetRenderState( D3DRS_STENCILFAIL, params.m_stencilFail );
      m_d3Device->SetRenderState( D3DRS_STENCILZFAIL, params.m_stencilZFail );
      m_d3Device->SetRenderState( D3DRS_STENCILPASS, params.m_stencilPass );
      m_d3Device->SetRenderState( D3DRS_STENCILFUNC, params.m_stencilFunc );
      m_d3Device->SetRenderState( D3DRS_STENCILREF, params.m_stencilRef );
      m_d3Device->SetRenderState( D3DRS_STENCILMASK, params.m_stencilMask );
      m_d3Device->SetRenderState( D3DRS_STENCILWRITEMASK, params.m_stencilWriteMask );
   }

   m_d3Device->SetPixelShader( m_dxPixelShader );
}

///////////////////////////////////////////////////////////////////////////////

void DX9PixelShader::endRendering()
{
   m_d3Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
   m_d3Device->SetRenderState( D3DRS_ZENABLE, true );
   m_d3Device->SetRenderState( D3DRS_ZWRITEENABLE, true );
   m_d3Device->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
   m_d3Device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
   m_d3Device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
   m_d3Device->SetRenderState( D3DRS_STENCILENABLE, false );
}

///////////////////////////////////////////////////////////////////////////////
