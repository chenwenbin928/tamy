#include "RPBStart.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPBStart );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< RPStartNode >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPBStart::RPBStart( RPStartNode& node )
   : TPipelineBlock< RPStartNode, RenderingPipelineNode >( node )
{
   setCaption( "Start" );
}

///////////////////////////////////////////////////////////////////////////////