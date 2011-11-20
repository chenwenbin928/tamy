#include "RPBTexture.h"
#include "core-Renderer/RPTextureNode.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPBTexture, RenderingPipelineBlock );
PROPERTY( TResourceHandle< RPTextureNode >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPBTexture::RPBTexture( RPTextureNode& node )
   : TRenderingPipelineBlock< RPTextureNode >( node )
{
   setCaption( "Texture" );
}

///////////////////////////////////////////////////////////////////////////////
