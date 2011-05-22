#include "core-Renderer\SpatialView.h"
#include "core-Renderer\SpatialRepresentation.h"
#include "core-Renderer\CameraContext.h"
#include "core-Renderer\Geometry.h"
#include "core\BoundingSphere.h"
#include "core\BoundingSpace.h"
#include "core\AABoundingBox.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

SpatialView::SpatialView( const AABoundingBox& treeBB )
   : m_storage( new RegularOctree< SpatialRepresentation >( treeBB ) )
   , m_visibilityTag( -1 )
{}

///////////////////////////////////////////////////////////////////////////////

SpatialView::~SpatialView()
{
   resetContents();

   delete m_storage; m_storage = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SpatialView::update( const CameraContext& camera )
{
   const BoundingVolume& volume = camera.getViewVolume();
   ++m_visibilityTag;

   Array< SpatialRepresentation *> visibleElems;
   m_storage->query(volume, visibleElems);

   unsigned int count = visibleElems.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      visibleElems[i]->tagAsVisible( m_visibilityTag );
   }

   for ( RepresentationsMap::iterator it = m_representations.begin(); it != m_representations.end(); ++it )
   {
      it->second->setVisible( m_visibilityTag );
   }
}

///////////////////////////////////////////////////////////////////////////////

void SpatialView::onEntityAdded( Entity& entity )
{
   Geometry* geometry = dynamic_cast< Geometry* >( &entity );
   if ( geometry == NULL )
   {
      return;
   }

   RepresentationsMap::iterator it = m_representations.find( geometry );
   if ( it != m_representations.end() )
   {
      return;
   }

   SpatialRepresentation* repr = new SpatialRepresentation( *geometry );
   m_representations.insert( std::make_pair( geometry, repr) );
   m_storage->insert( *repr );
}

///////////////////////////////////////////////////////////////////////////////

void SpatialView::onEntityRemoved(Entity& entity)
{
   Geometry* geometry = dynamic_cast< Geometry* >( &entity );
   if (geometry == NULL)
   {
      return;
   }

   RepresentationsMap::iterator it = m_representations.find( geometry );
   if ( it != m_representations.end() )
   {
      m_storage->remove( *it->second );
      delete it->second;
      m_representations.erase(it);
   }
}

///////////////////////////////////////////////////////////////////////////////

void SpatialView::onEntityChanged(Entity& entity)
{
}

///////////////////////////////////////////////////////////////////////////////

void SpatialView::resetContents()
{
   m_storage->clear();

   for ( RepresentationsMap::iterator it = m_representations.begin(); it != m_representations.end(); ++it )
   {
      delete it->second;
   }
   m_representations.clear();
}

///////////////////////////////////////////////////////////////////////////////