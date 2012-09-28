#include "RPBCamera.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPBCamera );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< RPCameraNode >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPBCamera::RPBCamera( RPCameraNode& node )
   : TGraphBlock< RPCameraNode, RenderingPipelineNode >( node )
{
   setCaption( "Camera" );
}

///////////////////////////////////////////////////////////////////////////////
