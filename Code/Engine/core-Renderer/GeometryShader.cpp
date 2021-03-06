#include "core-Renderer/GeometryShader.h"
#include "core-Renderer/GeometryShaderNode.h"
#include "core-Renderer/Renderer.h"
#include "core/RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( GeometryShader, tgsh, AM_BINARY )
   PROPERTY( std::vector< GeometryShaderNode* >, m_nodes );
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

GeometryShader::GeometryShader( const FilePath& resourceName )
   : Resource( resourceName )
{
}

///////////////////////////////////////////////////////////////////////////////

GeometryShader::~GeometryShader()
{
}

///////////////////////////////////////////////////////////////////////////////

void GeometryShader::onNodeAdded( GeometryShaderNode* node )
{
   addObject( node );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryShader::onNodeRemoved( GeometryShaderNode& node )
{
   removeObject( node.getObjectId() );
}

///////////////////////////////////////////////////////////////////////////////

void GeometryShader::onResourceLoaded( ResourcesManager& mgr )
{
   __super::onResourceLoaded( mgr );

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_nodes[i]->onGraphLoaded();
   }
}

///////////////////////////////////////////////////////////////////////////////
