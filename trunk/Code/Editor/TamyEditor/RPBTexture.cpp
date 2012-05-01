#include "RPBTexture.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPBTexture );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< RPTextureNode >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPBTexture::RPBTexture( RPTextureNode& node )
   : TPipelineBlock< RPTextureNode, RenderingPipelineNode >( node )
{
   setCaption( "Texture" );
}

///////////////////////////////////////////////////////////////////////////////
