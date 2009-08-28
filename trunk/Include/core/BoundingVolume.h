#pragma once

/// @file   core\BoundingVolume.h
/// @brief  common interface for inter-bounding volume collision tests

///////////////////////////////////////////////////////////////////////////////

struct AABoundingBox;
struct BoundingSphere;
struct Frustum;
struct Ray;
struct Triangle;
struct BoundingSpace;
struct D3DXMATRIX;
struct PointVolume;

///////////////////////////////////////////////////////////////////////////////

/**
 * This interface allows to check collisions between all available collision shapes
 */
class BoundingVolume
{
public:
   virtual ~BoundingVolume() {}

   virtual bool testCollision(const PointVolume& point) const = 0;

   virtual bool testCollision(const AABoundingBox& rhs) const = 0;

   virtual bool testCollision(const BoundingSphere& rhs) const = 0;

   virtual bool testCollision(const Frustum& rhs) const = 0;

   virtual bool testCollision(const Ray& rhs) const = 0;

   virtual bool testCollision(const Triangle& rhs) const = 0;

   /**
    * Collision with a bounding space always exists, as the space is
    * infinite and ubiquitous
    */
   bool testCollision(const BoundingSpace& rhs) const {return true;}

   virtual bool testCollision(const BoundingVolume& rhs) const = 0;

   virtual BoundingVolume* operator*(const D3DXMATRIX& mtx) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
