#include "core-Renderer/ShaderParam.h"


///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamFloat< EffectShader >::setParam( Renderer& renderer, void* shaderPtr )
{
}

///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamFloat< PixelShader >::setParam( Renderer& renderer, void* shaderPtr )
{
}

///////////////////////////////////////////////////////////////////////////////

template<>
void ShaderParamFloat< VertexShader >::setParam( Renderer& renderer, void* shaderPtr )
{
}

///////////////////////////////////////////////////////////////////////////////
