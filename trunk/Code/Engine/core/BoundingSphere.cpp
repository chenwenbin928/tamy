#include "core\BoundingSphere.h"
#include "core\Assert.h"
#include "core\CollisionTests.h"
#include "core\PointVolume.h"
#include "core\Plane.h"
#include "core\Matrix.h"
#include "core\AABoundingBox.h"


///////////////////////////////////////////////////////////////////////////////

BoundingSphere::BoundingSphere()
   : origin(0, 0, 0, 1)
   , radius(0)
{
}

///////////////////////////////////////////////////////////////////////////////

BoundingSphere::BoundingSphere(const Vector& _origin, float _radius)
   : origin( _origin )
   , radius( _radius )
{
   origin.w = 1;
}

///////////////////////////////////////////////////////////////////////////////

BoundingVolume* BoundingSphere::clone() const
{
   return new BoundingSphere( origin, radius );
}

///////////////////////////////////////////////////////////////////////////////

void BoundingSphere::transform( const Matrix& mtx, BoundingVolume& transformedVolume ) const
{
   // verify that the volume is a BoundingSphere
   ASSERT( dynamic_cast< BoundingSphere* >( &transformedVolume ) != NULL );
   BoundingSphere& transformedSphere = static_cast< BoundingSphere& >( transformedVolume );

   mtx.transform( origin, transformedSphere.origin );
   transformedSphere.radius = radius;
}

///////////////////////////////////////////////////////////////////////////////

void BoundingSphere::calculateBoundingBox( AABoundingBox& outBoundingBox ) const 
{
   Vector radVec( radius, radius, radius );
   outBoundingBox.min.setSub( origin, radVec );
   outBoundingBox.max.setAdd( origin, radVec );
}

///////////////////////////////////////////////////////////////////////////////

float BoundingSphere::distanceToPlane( const Plane& plane ) const
{
   // <fastfloat.todo>
   const FastFloat distance = plane.dotCoord( origin );
   FastFloat absDist;
   absDist.setAbs( distance );

   if ( absDist.getFloat() <= radius ) 
   { 
      return 0; 
   }
   else
   { 
      return distance.getFloat(); 
   }
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const PointVolume& rhs) const
{
   return ::testCollision(*this, rhs.point);
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const AABoundingBox& rhs) const
{
   return ::testCollision(rhs, *this);
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const BoundingSphere& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const Frustum& rhs) const
{
   return ::testCollision(rhs, *this);
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const Ray& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::testCollision(const Triangle& rhs) const
{
   ASSERT_MSG(false, "BoundingSphere::testCollision(const Triangle&) - Method not implemented");
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BoundingSphere::hasVolume() const
{
   return radius > 0;
}

///////////////////////////////////////////////////////////////////////////////
