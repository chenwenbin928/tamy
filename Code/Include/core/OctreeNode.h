/// @file   core\OctreeNode.h
/// @brief  an octree node
#ifndef _OCTREE_NODE_H
#define _OCTREE_NODE_H

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\AABoundingBox.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

template< typename OctreeElem >
class Octree;

///////////////////////////////////////////////////////////////////////////////

/**
 * A node of an octree.
 */
template <typename Elem>
class OctreeNode
{
   DECLARE_ALLOCATOR( OctreeNode< Elem >, AM_ALIGNED_16 );

private:
   enum
   {
      CS_FRONT_RIGHT_UPPER,
      CS_FRONT_RIGHT_LOWER,
      CS_FRONT_LEFT_UPPER,
      CS_FRONT_LEFT_LOWER,
      CS_BACK_RIGHT_UPPER,
      CS_BACK_RIGHT_LOWER,
      CS_BACK_LEFT_UPPER,
      CS_BACK_LEFT_LOWER,
   };

public:
   Array< Elem >                       m_elems;

   // bounds of the elements stored only in this node
   AABoundingBox                       m_localElementsBounds;

   // bounds of the elements stored in this node and the hierarchy that spans underneath
   AABoundingBox                       m_globalElementsBounds;

private:
   OctreeNode**                        m_children;
   Plane                               m_splitPlanes[3];
   unsigned int                        m_depth;

   // node bounds
   AABoundingBox                       m_bb;       


public:
   OctreeNode(const AABoundingBox& bb, unsigned int depth = 0);
   ~OctreeNode();

   /**
    * Checks whether this node intersects the specified bounding volume.
    *
    * @param vol  bounding volume we want to check the intersection with.
    */
   bool doesIntersect(const BoundingVolume& vol) const;

   /**
    * Subdivides the node, creating 8 child nodes under it - providing
    * the node isn't a parent node already.
    */
   void subdivide();

   /**
    * Returns the sector's children count
    */
   unsigned int getChildrenCount() const;

   /**
    * Returns a sector's child with the specified index.
    *
    * @param idx  child index
    */
   OctreeNode& getChild(unsigned int idx);

   /**
    * Returns a sector's child with the specified index (const version)
    *
    * @param idx  child index
    */
   const OctreeNode& getChild(unsigned int idx) const;

   /**
    * Returns the number of planes that split this type of node
    * into the sub-nodes.
    */
   unsigned int getSplitPlanesCount() const {return 3;}

   /**
    * Returns one of the split planes of the node.
    *
    * @param idx  split plane idx
    */
   const Plane& getSplitPlane(unsigned int idx) const;

   /**
    * Returns the tree depth this node is at.
    */
   inline unsigned int getDepth() const;

   /**
    * Returns the bounding box of the node.
    */
   inline const AABoundingBox& getBoundingBox() const;

   /**
    * Recalculates the boundaries of all stored elements
    *
    * @param octree     parent octree
    */
   template< typename OctreeElem >
   void recalculateGlobalBounds( const Octree< OctreeElem >& octree );

private:
   /**
    * Recalculates the boundaries of locally stored elements
    *
    * @param octree     parent octree
    */
   template< typename OctreeElem >
   void recalculateLocalBounds( const Octree< OctreeElem >& octree );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\OctreeNode.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _OCTREE_NODE_H
