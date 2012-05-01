#include "MBFloat.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MBFloat );
   PARENT( GraphBlock );
   PROPERTY( TResourceHandle< MNFloat >*, m_node );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

MBFloat::MBFloat( MNFloat& node )
   : TPipelineBlock< MNFloat, MaterialNode >( node )
{
   setCaption( "Float" );
}

///////////////////////////////////////////////////////////////////////////////
