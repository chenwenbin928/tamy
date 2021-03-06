/// @file   core-Renderer\RenderingView.h
/// @brief  view that renders renderable entities
#pragma once

#include <vector>
#include "core-MVC\ModelView.h"
#include "core\RegularOctree.h"
#include "core\AABoundingBox.h"


///////////////////////////////////////////////////////////////////////////////

class Renderer;
class Geometry;
class Light;
class Camera;
class RenderState;
class SceneRenderTreeBuilder;
class MemoryPool;
class RuntimeDataBuffer;
class AmbientLight;

///////////////////////////////////////////////////////////////////////////////

/**
* This view manages the visibility of renderables.
*/ 
class RenderingView : public ModelView
{
   DECLARE_ALLOCATOR( RenderingView, AM_ALIGNED_16 );

private:
   Renderer&                                                m_renderer;

   MemoryPool*                                              m_treeMemPool;
   RegularOctree< Geometry >*                               m_geometryStorage;
   RegularOctree< Light >*                                  m_lightsStorage;
   AmbientLight*                                            m_ambientLight;

public:
   /**
    * Constructor.
    */
   RenderingView( Renderer& renderer, const AABoundingBox& sceneBB );
   ~RenderingView();

   /**
    * Collects renderables that should be rendered this frame.
    *
    * @param outVisibleElems
    */
   void collectRenderables( Array< Geometry* >& outVisibleElems );

   /**
    * Collects renderables from the specified bounding volume
    *
    * @param volume
    * @param outVisibleElems
    */
   void collectRenderables( const BoundingVolume& volume, Array< Geometry* >& outVisibleElems ) const;

   /**
    * Collects visible lights that affect the scene and should be rendered this frame.
    *
    * @param outVisibleLights
    */
   void collectLights( Array< Light* >& outVisibleLights );

   /**
    * Collects lights from the specified bounding volume
    *
    * @param volume
    * @param outVisibleLights
    */
   void collectLights( const BoundingVolume& volume, Array< Light* >& outVisibleLights ) const;

   /**
    * Returns an ambient light instance that lights the scene.
    * There can be only one, or none at all. If more than one is added, only one will be selected to be rendered,
    * but which one will it be is undetermined.
    */
   inline AmbientLight* getAmbientLight() const { return m_ambientLight; }

   /**
    * Returns the scene bounds.
    *
    * @param outBounds
    */
   void getSceneBounds( AABoundingBox& outBounds ) const;

   // ----------------------------------------------------------------------
   // ModelView implementation
   // ----------------------------------------------------------------------
   void onEntityAdded( Entity& entity );
   void onEntityRemoved( Entity& entity );
   void onEntityChanged( Entity& entity );

protected:
   void resetContents();
};

///////////////////////////////////////////////////////////////////////////////
