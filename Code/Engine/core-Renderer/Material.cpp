#include "core-Renderer/Material.h"
#include "core-Renderer/MaterialEntity.h"
#include "core-Renderer/MaterialNode.h"
#include "core-Renderer/Renderer.h"
#include "core/RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( Material, tmat, AM_BINARY )
   PROPERTY( std::vector< MaterialNode* >, m_nodes );
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

Material::Material( const FilePath& resourceName )
   : Resource( resourceName )
{
}

///////////////////////////////////////////////////////////////////////////////

Material::~Material()
{
}

///////////////////////////////////////////////////////////////////////////////

void Material::onNodeAdded( MaterialNode* node )
{
   addObject( node );
}

///////////////////////////////////////////////////////////////////////////////

void Material::onNodeRemoved( MaterialNode& node )
{
   removeObject( node.getObjectId() );
}

///////////////////////////////////////////////////////////////////////////////

void Material::onResourceLoaded( ResourcesManager& mgr )
{
   __super::onResourceLoaded( mgr );

   uint count = m_nodes.size();
   for ( uint i = 0; i < count; ++i )
   {
      m_nodes[i]->onGraphLoaded();
   }
}

///////////////////////////////////////////////////////////////////////////////
