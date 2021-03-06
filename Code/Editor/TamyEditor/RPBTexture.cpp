#include "RPBTexture.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPBTexture );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< RPTextureNode >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPBTexture::RPBTexture( RPTextureNode& node )
   : TGraphBlock< RPTextureNode, RenderingPipelineNode >( node )
{
   setCaption( "Texture" );
}

///////////////////////////////////////////////////////////////////////////////
