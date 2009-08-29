#pragma once

/// @file   core\SpatialStorage.h
/// @brief  a spatial storage interface

#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

class BoundingVolume;

///////////////////////////////////////////////////////////////////////////////

/**
 * A spatial storage stores elements that have a position in space and a volume
 * in an efficient manner, optimized for query time.
 *
 * An example of a spatial storage would be an octree, a quad tree etc.
 */
template<typename Elem>
class SpatialStorage
{
public:
   virtual ~SpatialStorage() {}

   /**
    * The method inserts a new element into the storage.
    *
    * @param elem    element we want to add
    */
   virtual void insert(Elem& elem) = 0;

   /**
    * The method allows to remove an element from the storage.
    *
    * @param elem    element we want to remove
    */
   virtual void remove(Elem& elem) = 0;

   /**
    * This method allows to query all the elements that are contained
    * in the passed bounding volume.
    *
    * @param boundingVol   volume that bounds the elements we want
    * @param output        upon method return this array will be filled with
    *                      elements overlapping the query volume
    */
   virtual void query(const BoundingVolume& boundingVol, 
                      Array<Elem*>& output) const = 0;
};

///////////////////////////////////////////////////////////////////////////////