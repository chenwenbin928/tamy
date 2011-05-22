#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\VertexShader.h"
#include "core-Renderer\Renderer.h"
#include "core\ResourcesManager.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RTTI( FullscreenQuad )
END_RTTI

///////////////////////////////////////////////////////////////////////////////

FullscreenQuad::FullscreenQuad()
{
}

///////////////////////////////////////////////////////////////////////////////

FullscreenQuad::~FullscreenQuad()
{
}

///////////////////////////////////////////////////////////////////////////////

void FullscreenQuad::initialize( ResourcesManager& rm )
{
}

///////////////////////////////////////////////////////////////////////////////

void FullscreenQuad::render( unsigned int width, unsigned int height )
{
   impl().render( width, height );
}

///////////////////////////////////////////////////////////////////////////////