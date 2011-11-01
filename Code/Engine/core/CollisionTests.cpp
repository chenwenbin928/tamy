#include "core\CollisionTests.h"
#include "core\AABoundingBox.h"
#include "core\Ray.h"
#include "core\BoundingSphere.h"
#include "core\Frustum.h"
#include "core\Triangle.h"
#include "core\PointVolume.h"
#include "core\QuadraticEquations.h"


///////////////////////////////////////////////////////////////////////////////

static bool areRangesOverlapping(float min1, float max1, float min2, float max2)
{
   return !((max1 < min2) || (max2 < min1));
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const AABoundingBox& aabb, const D3DXVECTOR3& point)
{
   return areRangesOverlapping(aabb.min.x, aabb.max.x, point.x, point.x) && 
      areRangesOverlapping(aabb.min.y, aabb.max.y, point.y, point.y) && 
      areRangesOverlapping(aabb.min.z, aabb.max.z, point.z, point.z);
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const AABoundingBox& aabb1, const AABoundingBox& aabb2)
{
   UINT collidingPlanes = 0;
   if (areRangesOverlapping(aabb1.min.x, aabb1.max.x, aabb2.min.x, aabb2.max.x))
   {
      collidingPlanes++;
   }

   if (areRangesOverlapping(aabb1.min.y, aabb1.max.y, aabb2.min.y, aabb2.max.y))
   {
      collidingPlanes++;
   }

   if (areRangesOverlapping(aabb1.min.z, aabb1.max.z, aabb2.min.z, aabb2.max.z))
   {
      collidingPlanes++;
   }

   return (collidingPlanes == 3);
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const AABoundingBox& aabb, const BoundingSphere& sphere)
{
   D3DXPLANE plane(0, 0, -1, aabb.min.z);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   plane = D3DXPLANE(0, 0,  1, -aabb.max.z);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   plane = D3DXPLANE(0, -1, 0, aabb.min.y);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   plane = D3DXPLANE(0,  1, 0, -aabb.max.y);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   plane = D3DXPLANE(-1, 0, 0, aabb.min.x);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   plane = D3DXPLANE( 1, 0, 0, -aabb.max.x);
   if (D3DXPlaneDotCoord(&plane, &sphere.origin) > sphere.radius) {return false;}

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const AABoundingBox& aabb, const Frustum& frustum)
{
   D3DXVECTOR3 nearPoint;

   for (int i =0; i < 6; ++i)
   {
      D3DXVECTOR3 planeNormal(frustum.planes[i].a, frustum.planes[i].b, frustum.planes[i].c);

      if (planeNormal.x > 0.0f)
      {
         if (planeNormal.y > 0.0f)
         {
            if (planeNormal.z > 0.0f)
            {
               nearPoint.x = aabb.min.x; nearPoint.y = aabb.min.y; nearPoint.z = aabb.min.z;
            }
            else
            {
               nearPoint.x = aabb.min.x; nearPoint.y = aabb.min.y; nearPoint.z = aabb.max.z;
            }
         }
         else
         {
            if (planeNormal.z> 0.0f)
            {
               nearPoint.x = aabb.min.x; nearPoint.y = aabb.max.y; nearPoint.z = aabb.min.z;
            }
            else
            {
               nearPoint.x = aabb.min.x; nearPoint.y = aabb.max.y; nearPoint.z = aabb.max.z;
            }
         }
      }
      else 
      {
         if (planeNormal.y > 0.0f)
         {
            if (planeNormal.z > 0.0f)
            {
               nearPoint.x = aabb.max.x; nearPoint.y = aabb.min.y; nearPoint.z = aabb.min.z;
            }
            else
            {
               nearPoint.x = aabb.max.x; nearPoint.y = aabb.min.y; nearPoint.z = aabb.max.z;
            }
         }
         else
         {
            if (planeNormal.z > 0.0f)
            {
               nearPoint.x = aabb.max.x; nearPoint.y = aabb.max.y; nearPoint.z = aabb.min.z;
            }
            else
            {
               nearPoint.x = aabb.max.x; nearPoint.y = aabb.max.y; nearPoint.z = aabb.max.z;
            }
         }
      }

      float dist = D3DXVec3Dot( &planeNormal, &nearPoint );
      dist = dist + frustum.planes[i].d;
      if ( dist > 0 )
      {
         return false;
      }
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const Frustum& frustum, const BoundingSphere& sphere)
{
   for (int i = 0; i < 6; ++i)
   {
      if (D3DXPlaneDotCoord(&(frustum.planes[i]),  &(sphere.origin)) > sphere.radius) {return false;}
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const AABoundingBox& aabb, const Ray& ray)
{
   return (rayToAABBDistance(ray, aabb) < FLT_MAX);
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const Ray& ray, const D3DXPLANE& plane, D3DXVECTOR3& intersectionPt)
{
   float t = rayToPlaneDistance(ray, plane);

   if (t >= FLT_MAX) {return false;}

   intersectionPt = ray.origin + (ray.direction * t);

   return true;
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const BoundingSphere& sphere, const D3DXVECTOR3& point)
{
   D3DXVECTOR3 to = sphere.origin - point;
   return (D3DXVec3LengthSq(&to) < sphere.radius * sphere.radius);
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const BoundingSphere& sphere, const BoundingSphere& rhs)
{
   D3DXVECTOR3 to = sphere.origin - rhs.origin;
   float totalRadius  = sphere.radius + rhs.radius;

   return ((&sphere != &rhs) && (D3DXVec3LengthSq(&to) < totalRadius * totalRadius));
}

///////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 findIntersectionRemovalVector(const BoundingSphere& sphere, const BoundingSphere& colidor)
{
   D3DXVECTOR3 toColidor = colidor.origin - sphere.origin;
   float distFromEachOther = D3DXVec3Length(&toColidor);

   float amountOfOverLap = colidor.radius + sphere.radius - distFromEachOther;
   if (amountOfOverLap >= 0)
   {
      return ((toColidor / distFromEachOther) * amountOfOverLap);
   }
   else
   {
      return D3DXVECTOR3(0, 0, 0);
   }
}

///////////////////////////////////////////////////////////////////////////////


D3DXVECTOR3 findIntersectionRemovalVector(const AABoundingBox& aabb, const AABoundingBox& colidor)
{
   D3DXVECTOR3 vec(0, 0, 0);

   if (areRangesOverlapping(aabb.min.x, aabb.max.x, colidor.max.x, colidor.max.x))
   {
      vec.x = aabb.min.x - colidor.max.x;
   }
   else if (areRangesOverlapping(aabb.min.x, aabb.max.x, colidor.min.x, colidor.min.x))
   {
      vec.x = aabb.max.x - colidor.min.x;
   }

   if (areRangesOverlapping(aabb.min.z, aabb.max.z, colidor.max.z, colidor.max.z))
   {
      vec.z = aabb.min.z - colidor.max.z;
   }
   else if (areRangesOverlapping(aabb.min.z, aabb.max.z, colidor.min.z, colidor.min.z))
   {
      vec.z = aabb.max.z - colidor.min.z;
   }

   return vec;
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const BoundingSphere& sphere, const Ray& ray)
{
   return (rayToBSDistance(ray, sphere) < FLT_MAX);
}

///////////////////////////////////////////////////////////////////////////////

bool testCollision(const Ray& ray, const Triangle& triangle)
{
   return (bool)D3DXIntersectTri(&triangle.vertex(0), &triangle.vertex(1), &triangle.vertex(2),
      &ray.origin, &ray.direction, NULL, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////

float rayToPlaneDistance(const Ray& ray, const D3DXPLANE& plane)
{
   float planeDistance = D3DXPlaneDotCoord(&plane, &ray.origin);
   float projRayLength = D3DXPlaneDotNormal(&plane, &ray.direction);

   float t;
   if (projRayLength == 0)
   {
      if (planeDistance == 0)
      {
         t = 0;
      }
      else
      {
         t = -1;
      }
   }
   else
   {
      t = - (planeDistance / projRayLength);
   }

   if (t < 0) 
   {
      return FLT_MAX;
   }
   else
   {
      return t;
   }
}

///////////////////////////////////////////////////////////////////////////////

float rayToPointDistance(const Ray& ray, const D3DXVECTOR3& point)
{
   D3DXVECTOR3 vecToPt = point - ray.origin;

   float t = D3DXVec3Dot(&vecToPt, &ray.direction);
   if (t < 0)
   {
      // if the point lies behind the ray's origin, the distance is infinite
      return FLT_MAX;
   }

   // check if the projected point lies on the ray
   D3DXVECTOR3 resultPt = t * ray.direction + ray.origin;
   resultPt -= vecToPt;
   if (D3DXVec3LengthSq(&resultPt) < 1e-3)
   {
      // if it does - return the calculated distance
      return t;
   }
   else
   {
      // if not - it's not on the ray, so the distance to it is infinite
      return FLT_MAX;
   }
}

///////////////////////////////////////////////////////////////////////////////

float rayToAABBDistance(const Ray& ray, const AABoundingBox& aabb)
{
   // 'slabs' algorithm
   float tMin = -FLT_MAX;
   float tMax = FLT_MAX;
   float t1, t2, fTemp;

   D3DXVECTOR3 extentsMin = aabb.min - ray.origin;
   D3DXVECTOR3 extentsMax = aabb.max - ray.origin;

   for (int i = 0; i < 3; ++i)
   {
      if (fabsf(ray.direction[i]) > 1e-3f)
      {
         fTemp = 1.0f / ray.direction[i];
         t1 = extentsMax[i] * fTemp;
         t2 = extentsMin[i] * fTemp;
         if (t1 > t2) {fTemp = t1; t1 = t2; t2 = fTemp; }

         if (t1 > tMin) tMin = t1;
         if (t2 < tMax) tMax = t2;

         if (tMin > tMax) return FLT_MAX;
         if (tMax < 0) return FLT_MAX;
      }
      else
      {
         if ((ray.origin[i] < aabb.min[i]) || (ray.origin[i] > aabb.max[i]))
         {
            return FLT_MAX;
         }
      }
   }

   if (tMin < 0) tMin = 0;
   return tMin;
}

///////////////////////////////////////////////////////////////////////////////

float rayToBSDistance(const Ray& ray, const BoundingSphere& bs)
{
   float a, b, c, l2, t;

   D3DXVECTOR3 L = ray.origin - bs.origin;
   l2 = D3DXVec3LengthSq(&L);

   a = D3DXVec3LengthSq(&ray.direction);
   b = 2.0f * D3DXVec3Dot(&ray.direction, &L);
   c = l2 - (bs.radius * bs.radius);

   if (c < 0.0f) 
   {
      // the ray starts inside the sphere
      return 0;
   }

   if (!solveQuadratic(a, b, c, t)) 
   {
      // the sphere doesn't intersect the plane
      return FLT_MAX;
   }

   return t;
}

///////////////////////////////////////////////////////////////////////////////
