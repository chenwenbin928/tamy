#pragma once

/// @file   core-Renderer\SpatialEntity.h
/// @brief  abstract entity that has a location in 3D space

#include "core\Node.h"
#include "core-MVC\Entity.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

/**
 * Abstract entity that has a location in 3D space.
 */
class SpatialEntity : public Entity, public Node
{
   DECLARE_CLASS(SpatialEntity)

public:
   /**
    * Constructor.
    *
    * @param name    node name
    */
   SpatialEntity( const std::string& name = "" );
   virtual ~SpatialEntity();

protected:
   // -------------------------------------------------------------------------
   // Entity implementation
   // -------------------------------------------------------------------------
   void onChildAttached(Entity& child);
   void onChildDetached(Entity& child);
};

///////////////////////////////////////////////////////////////////////////////