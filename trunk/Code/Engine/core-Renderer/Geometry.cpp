#include "core-Renderer\Geometry.h"
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\RenderState.h"
#include "core-MVC\SpatialEntity.h"
#include "core-MVC.h"
#include "core.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( Geometry, SpatialEntity )
   PROPERTY_EDIT( "resource", GeometryResource*, m_resource )
END_OBJECT()


///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry()
   : m_resource(NULL)
   , m_globalBounds( new PointVolume( D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) ) )
{
}

///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry( const Geometry& rhs )
   : SpatialEntity( rhs )
   , m_globalBounds( new PointVolume( D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) ) )
{
}

///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry( GeometryResource& resource )
   : m_resource( &resource )
   , m_globalBounds( resource.getBoundingVolume().clone() )
{
}

///////////////////////////////////////////////////////////////////////////////

Geometry::~Geometry()
{
   m_resource = NULL;
   m_states.clear();

   delete m_globalBounds;
   m_globalBounds = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::render( Renderer& renderer )
{
   if ( m_resource )
   {
      onPreRender( renderer );
      m_resource->render( renderer );
      onPostRender( renderer );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::addState( RenderState& state )
{
   // we don't want duplicates - so look for one
   RenderStatesVec::const_iterator it = std::find( m_states.begin(), m_states.end(), &state );
   if ( it == m_states.end() )
   {
      // add the state
      m_states.push_back( &state );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::removeState( RenderState& state )
{
   // find the position of the requested state
   RenderStatesVec::const_iterator it = std::find( m_states.begin(), m_states.end(), &state );
   if ( it != m_states.end() )
   {
      // remove the state
      m_states.erase( it );
   }
}

///////////////////////////////////////////////////////////////////////////////

const BoundingVolume& Geometry::calculateBoundingVolume() const
{
   if ( m_resource )
   {
      const BoundingVolume& geomBoundingVol = m_resource->getBoundingVolume();

      const D3DXMATRIX& parentMtx = getGlobalMtx();
      geomBoundingVol.transform( parentMtx, *m_globalBounds );
   }

   return *m_globalBounds;
}

///////////////////////////////////////////////////////////////////////////////

std::string Geometry::getGeometryName() const
{
   if ( m_resource )
   {
      return m_resource->getResourceName();
   }
   else
   {
      return "<<uninitialized geometry>>";
   }
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::onAttached( Entity& parent )
{
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::onDetached( Entity& parent )
{
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::onObjectLoaded()
{
   __super::onObjectLoaded();

   // refresh the global bounds
   delete m_globalBounds;
   if ( m_resource != NULL )
   {
      m_globalBounds = m_resource->getBoundingVolume().clone();
   }
   else
   {
      m_globalBounds = new PointVolume( D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

Entity* Geometry::cloneSelf() const
{
   Geometry* entity = new Geometry( *this );
   return entity;
}

///////////////////////////////////////////////////////////////////////////////