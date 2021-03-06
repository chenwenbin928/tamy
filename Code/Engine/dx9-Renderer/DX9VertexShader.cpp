#include "dx9-Renderer\DX9VertexShader.h"
#include "dx9-Renderer\DX9Renderer.h"
#include "core-Renderer\ShaderTexture.h"
#include "core\File.h"
#include "dx9-Renderer\DXErrorParser.h"
#include <stdexcept>
#include "dx9-Renderer/DX9ShaderIncludeLoader.h"


///////////////////////////////////////////////////////////////////////////////

void RCBindVertexShader::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   DX9VertexShader* dxShader = dxRenderer.getVertexShader( m_shader );
   if ( dxShader )
   {
      // set the shader parameters
      dxShader->setActiveTechnique( m_techniqueIdx );
      setParams( renderer, dxShader );

      dxShader->beginRendering( m_techniqueIdx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RCUnbindVertexShader::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   DX9VertexShader* dxShader = dxRenderer.getVertexShader( m_shader );
   if ( dxShader )
   {
      dxShader->endRendering();
      dxShader->setActiveTechnique( -1 );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DX9VertexShader::DX9VertexShader( const DX9Renderer& renderer, const VertexShader& shader )
   : m_shader( shader )
   , m_renderer( renderer )
   , m_d3Device( &renderer.getD3Device() )
   , m_shaderConstants( NULL )
   , m_vertexDecl( NULL )
   , m_activeConstantsTable( NULL )
{
   initialize();
}

///////////////////////////////////////////////////////////////////////////////

DX9VertexShader::~DX9VertexShader()
{
   m_d3Device = NULL;

   uint count = m_dxVertexShader.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( m_dxVertexShader[i] != NULL )
      {
         m_dxVertexShader[i]->Release();
      }

      if ( m_shaderConstants[i] )
      {
         m_shaderConstants[i]->Release();
      }
   }
   m_dxVertexShader.clear();
   m_shaderConstants.clear();

   if ( m_vertexDecl )
   {
      m_vertexDecl->Release();
      m_vertexDecl = NULL;
   }

   m_activeConstantsTable = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::initialize()
{
   // load the effect
   const std::string& shaderContents = m_shader.getScript();
   const char* shaderProfile = D3DXGetVertexShaderProfile( m_d3Device );

   DWORD flags = 0;
#ifdef _DEBUG
   flags = D3DXSHADER_DEBUG;
#endif

   // compile all included techniques
   ASSERT_MSG( m_dxVertexShader.empty(), "There are still some initialized vertex shaders left behind" );

   uint techniquesCount = m_shader.getTechniquesCount();
   for ( uint techniqueIdx = 0; techniqueIdx < techniquesCount; ++techniqueIdx )
   {
      ID3DXBuffer* shaderBuf = NULL;
      ID3DXBuffer* errorsBuf = NULL;
      ID3DXConstantTable* constantsTable = NULL;
      DX9ShaderIncludeLoader includesLoader;
      HRESULT res = D3DXCompileShader(
         shaderContents.c_str(), 
         shaderContents.length(),
         NULL,                            // defines
         &includesLoader,
         m_shader.getEntryFunctionName( techniqueIdx ).c_str(),
         shaderProfile, 
         flags,
         &shaderBuf, 
         &errorsBuf,
         &constantsTable );

      if ( FAILED(res) || shaderBuf == NULL )
      {
         if ( errorsBuf != NULL )
         {
            std::string compilationErrors = ( const char* )errorsBuf->GetBufferPointer();
            errorsBuf->Release();
            std::string errMsg = std::string( "Shader compilation error: " ) + compilationErrors;
            ASSERT_MSG( false, errMsg.c_str() );
            return;
         }
         else
         {
            std::string errMsg = translateDxError( "Error while compiling a shader", res );
            ASSERT_MSG( false, errMsg.c_str() );
            return;
         }
      }

      IDirect3DVertexShader9* dxVertexShader = NULL;
      res = m_d3Device->CreateVertexShader( ( const DWORD* )shaderBuf->GetBufferPointer(), &dxVertexShader );
      shaderBuf->Release();
      if ( FAILED(res) )
      {
         std::string errMsg = translateDxError( "Error while creating a shader", res );
         ASSERT_MSG( false, errMsg.c_str() );
         dxVertexShader = NULL;
      }

      m_dxVertexShader.push_back( dxVertexShader );
      m_shaderConstants.push_back( constantsTable );
   }

   m_activeConstantsTable = NULL;

   // create the vertex declaration
   // <renderer.todo> right now VertexDescriptor is an exact replica of D3DVERTEXELEMENT9 - but this needs
   // to change in  order to support OpenGL, DX11 etc.
   HRESULT res = m_d3Device->CreateVertexDeclaration( (const D3DVERTEXELEMENT9 *)m_shader.getVerexDescription(), &m_vertexDecl );
   if ( FAILED( res ) )
   {
      ASSERT_MSG( false, "Can't create a vertex declaration" );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setActiveTechnique( int techniqueIdx )
{
   if ( techniqueIdx >= 0 )
   {
      m_activeConstantsTable = m_shaderConstants[ techniqueIdx ];
   }
   else
   {
      m_activeConstantsTable = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setBool( const char* paramName, bool val )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      m_activeConstantsTable->SetBool( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setInt( const char* paramName, int val )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      m_activeConstantsTable->SetInt( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setIntArray( const char* paramName, int* valsArr, unsigned int size )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_activeConstantsTable->SetIntArray( m_d3Device, hConstant, valsArr, size );
         ASSERT( SUCCEEDED( res ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setFloat( const char* paramName, float val )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      m_activeConstantsTable->SetFloat( m_d3Device, hConstant, val );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setFloatArray( const char* paramName, float* valsArr, unsigned int size )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_activeConstantsTable->SetFloatArray( m_d3Device, hConstant, valsArr, size );
         ASSERT( SUCCEEDED( res ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setMtx( const char* paramName, const D3DXMATRIX& matrix )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_activeConstantsTable->SetMatrix( m_d3Device, hConstant, &matrix );
         ASSERT_MSG( SUCCEEDED( res ), translateDxError( "setMtx", res ).c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setMtxArray( const char* paramName, const D3DXMATRIX* matrices, unsigned int size )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_activeConstantsTable->SetMatrixArray( m_d3Device, hConstant, matrices, size );
         ASSERT_MSG( SUCCEEDED( res ), translateDxError( "setMtxArray", res ).c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setVec4( const char* paramName, const D3DXVECTOR4& vec )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      if ( hConstant )
      {
         HRESULT res = m_activeConstantsTable->SetVector( m_d3Device, hConstant, &vec );
         ASSERT_MSG( SUCCEEDED( res ), translateDxError( "setVec4", res ).c_str() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setVec4Array( const char* paramName, const D3DXVECTOR4* vecArr, unsigned int size )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      HRESULT res = m_activeConstantsTable->SetVectorArray( m_d3Device, hConstant, vecArr, size );
      ASSERT_MSG( SUCCEEDED( res ), translateDxError( "setVec4Array", res ).c_str() );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::setTexture( const char* paramName, IDirect3DTexture9* texture )
{
   if ( m_activeConstantsTable )
   {
      D3DXHANDLE hConstant = m_activeConstantsTable->GetConstantByName( NULL, paramName );
      UINT samplerIdx = m_activeConstantsTable->GetSamplerIndex( hConstant );
      
      m_d3Device->SetTexture( samplerIdx, texture );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::beginRendering( uint techniqueIdx )
{
   ASSERT( m_vertexDecl != NULL );
   if ( m_vertexDecl )
   {
      m_d3Device->SetVertexDeclaration( m_vertexDecl );
      m_d3Device->SetVertexShader( m_dxVertexShader[techniqueIdx] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DX9VertexShader::endRendering()
{
   m_d3Device->SetVertexShader( NULL );
}

///////////////////////////////////////////////////////////////////////////////
