#include "core-Renderer/ShaderParam.h"
#include "dx9-Renderer/DX9RenderTarget.h"
#include "dx9-Renderer/DX9Renderer.h"
#include "dx9-Renderer/DX9PixelShader.h"
#include "dx9-Renderer/DX9VertexShader.h"
#include <d3d9.h>


///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamRenderTarget< EffectShader >::setParam( Renderer& renderer, void* shaderPtr )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   IDirect3DTexture9* texture = dxRenderer.getRenderTarget( m_val )->getDxTexture();

   ID3DXEffect* dxEffect = reinterpret_cast< ID3DXEffect* >( shaderPtr );
   dxEffect->SetTexture( m_name.c_str(), texture );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamRenderTarget< PixelShader >::setParam( Renderer& renderer, void* shaderPtr )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   IDirect3DTexture9* texture = dxRenderer.getRenderTarget( m_val )->getDxTexture();

   DX9PixelShader* dxPixelShader = reinterpret_cast< DX9PixelShader* >( shaderPtr );
   dxPixelShader->setTexture( m_name.c_str(), texture );
}

///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamRenderTarget< VertexShader >::setParam( Renderer& renderer, void* shaderPtr )
{
   DX9Renderer& dxRenderer = static_cast< DX9Renderer& >( renderer );
   IDirect3DTexture9* texture = NULL; dxRenderer.getRenderTarget( m_val )->getDxTexture();

   DX9VertexShader* dxVertexShader = reinterpret_cast< DX9VertexShader* >( shaderPtr );
   dxVertexShader->setTexture( m_name.c_str(), texture );
}

///////////////////////////////////////////////////////////////////////////////
