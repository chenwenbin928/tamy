#include "core.h"
#include "core\AABoundingBox.h"
#include "core\Assert.h"
#include "core\CollisionTests.h"
#include "core\PointVolume.h"
#include "core\Matrix.h"
#include "core\Plane.h"


///////////////////////////////////////////////////////////////////////////////

AABoundingBox::AABoundingBox()
{
   min.set( FLT_MAX, FLT_MAX, FLT_MAX );
   max.set( -FLT_MAX, -FLT_MAX, -FLT_MAX );
}

///////////////////////////////////////////////////////////////////////////////

AABoundingBox::AABoundingBox( const Vector& _min, const Vector& _max )
{
   min = _min;
   max = _max;
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::setExpanded( const AABoundingBox& bounds, const FastFloat& multiplier )
{
   min.setMul( bounds.min, multiplier );
   max.setMul( bounds.max, multiplier );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::add( const AABoundingBox& otherBox, AABoundingBox& unionBox ) const
{
   unionBox.min.setMin( unionBox.min, min );
   unionBox.max.setMax( unionBox.max, max );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::getExtents( Vector& outExtents ) const
{
   outExtents.setSub( max, min );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::getCenter( Vector& outCenter ) const
{
   outCenter.setAdd( min, max );
   outCenter.mul( Float_Inv2 );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::reset()
{
   min.setBroadcast( Float_INF );
   max.setBroadcast( Float_NegINF );
}

///////////////////////////////////////////////////////////////////////////////

BoundingVolume* AABoundingBox::clone() const
{
   return new AABoundingBox( min, max );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::transform( const Matrix& mtx, BoundingVolume& transformedVolume ) const
{
   // verify that the volume is an AABoundingBox
   ASSERT( dynamic_cast< AABoundingBox* >( &transformedVolume ) != NULL );
   AABoundingBox& transformedBox = static_cast< AABoundingBox& >( transformedVolume );

   // begin at transform position
   transformedBox.min = mtx.position();
   transformedBox.max = transformedBox.min;

   // find extreme points by considering product of 
   // min and max with each component of mtx.
   float a, b;
   ALIGN_16 float tMin[4];
   ALIGN_16 float tMax[4];
   memset( tMin, 0, sizeof( float ) * 4 );
   memset( tMax, 0, sizeof( float ) * 4 );
   for( uint j = 0; j < 3; ++j )
   {
      for( uint i = 0; i < 3; ++i )
      {
         a = mtx( i, j ) * min[i];
         b = mtx( i, j ) * max[i];

         if( a < b )
         {
            tMin[j] += a;
            tMax[j] += b;
         }
         else
         {
            tMin[j] += b;
            tMax[j] += a;
         }
      }
   }

   transformedBox.min.set( tMin );
   transformedBox.max.set( tMax );
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::calculateBoundingBox( AABoundingBox& outBoundingBox ) const
{
   outBoundingBox.min = min;
   outBoundingBox.max = max;
}

///////////////////////////////////////////////////////////////////////////////

const FastFloat AABoundingBox::distanceToPlane( const Plane& plane ) const
{
   Vector planeNormal;
   plane.getNormal( planeNormal );

   Vector nearPoint, farPoint;

   // <fastfloat.todo> selectors
   if ( planeNormal[0] > 0.0f ) 
   {
      farPoint[0] = max[0]; nearPoint[0] = min[0];
   }
   else 
   {
      farPoint[0] = min[0]; nearPoint[0] = max[0];
   }

   if ( planeNormal[1] > 0.0f ) 
   {
      farPoint[1] = max[1]; nearPoint[1] = min[1];
   }
   else 
   {
      farPoint[1] = min[1]; nearPoint[1] = max[1];
   }


   if (planeNormal[2] > 0.0f) 
   {
      farPoint[2] = max[2]; nearPoint[2] = min[2];
   }
   else 
   {
      farPoint[2] = min[2]; nearPoint[2] = max[2];
   }

   if ( plane.dotCoord( nearPoint ) > Float_0 )
   {
      // the box is in front
      return Float_1;
   }
   if ( plane.dotCoord( farPoint ) >= Float_0 )
   {
      return Float_0;
   }
   else
   {
      // the box is behind
      return Float_Minus1;
   }
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const PointVolume& point) const
{
   return ::testCollision(*this, point.point);
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const AABoundingBox& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const BoundingSphere& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const Frustum& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const Ray& rhs) const
{
   return ::testCollision(*this, rhs);
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::testCollision(const Triangle& rhs) const
{
   ASSERT_MSG(false, "AABoundingBox::testCollision(const Triangle&) - Method not implemented");
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::hasVolume() const
{
   return (bool)(min != max);
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::include( const Vector& pt )
{
   min.setMin( pt, min );
   max.setMax( pt, max );
}

///////////////////////////////////////////////////////////////////////////////
