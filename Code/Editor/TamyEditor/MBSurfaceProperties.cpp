#include "MBSurfaceProperties.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MBSurfaceProperties );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< MNSurfaceProperties >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MBSurfaceProperties::MBSurfaceProperties( MNSurfaceProperties& node )
   : TPipelineBlock< MNSurfaceProperties, MaterialNode >( node )
{
   setCaption( "Surf. props" );
}

///////////////////////////////////////////////////////////////////////////////