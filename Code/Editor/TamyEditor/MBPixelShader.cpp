#include "MBPixelShader.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MBPixelShader );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< MNPixelShader >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MBPixelShader::MBPixelShader( MNPixelShader& node )
   : TPipelineBlock< MNPixelShader, MaterialNode >( node )
{
   setCaption( "Pixel shader" );
}

///////////////////////////////////////////////////////////////////////////////