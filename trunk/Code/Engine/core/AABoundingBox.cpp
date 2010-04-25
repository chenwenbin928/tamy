#include "core\AABoundingBox.h"
#include "core\Assert.h"
#include "core\CollisionTests.h"
#include "core\PointVolume.h"


///////////////////////////////////////////////////////////////////////////////

AABoundingBox::AABoundingBox()
{
   min = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
   max = D3DXVECTOR3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

///////////////////////////////////////////////////////////////////////////////

AABoundingBox::AABoundingBox(const D3DXVECTOR3& _min, const D3DXVECTOR3& _max)
{
   min = _min;
   max = _max;
}

///////////////////////////////////////////////////////////////////////////////

AABoundingBox AABoundingBox::operator+(const AABoundingBox& rhs) const
{
   AABoundingBox newBB;

   newBB.min.x = this->min.x < rhs.min.x ? this->min.x : rhs.min.x;
   newBB.min.y = this->min.y < rhs.min.y ? this->min.y : rhs.min.y;
   newBB.min.z = this->min.z < rhs.min.z ? this->min.z : rhs.min.z;

   newBB.max.x = this->max.x > rhs.max.x ? this->max.x : rhs.max.x;
   newBB.max.y = this->max.y > rhs.max.y ? this->max.y : rhs.max.y;
   newBB.max.z = this->max.z > rhs.max.z ? this->max.z : rhs.max.z;

   return newBB;
}

///////////////////////////////////////////////////////////////////////////////

BoundingVolume* AABoundingBox::operator*(const D3DXMATRIX& mtx) const
{
   AABoundingBox* newBB = new AABoundingBox();

   D3DXVec3TransformCoord(&newBB->min, &min, &mtx);
   D3DXVec3TransformCoord(&newBB->max, &max, &mtx);

   return newBB;
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::operator*=(const D3DXMATRIX& mtx)
{
   D3DXVec3TransformCoord(&min, &min, &mtx);
   D3DXVec3TransformCoord(&max, &max, &mtx);
}

///////////////////////////////////////////////////////////////////////////////

float AABoundingBox::distanceToPlane(const D3DXPLANE& plane) const
{
   D3DXVECTOR3 planeNormal(plane.a, plane.b, plane.c);

   D3DXVECTOR3 nearPoint, farPoint;

   if (planeNormal.x > 0.0f) {farPoint.x = max.x; nearPoint.x = min.x;}
   else {farPoint.x = min.x; nearPoint.x = max.x;}
   if (planeNormal.y > 0.0f) {farPoint.y = max.y; nearPoint.y = min.y;}
   else {farPoint.y = min.y; nearPoint.y = max.y;}
   if (planeNormal.z > 0.0f) {farPoint.z = max.z; nearPoint.z = min.z;}
   else {farPoint.z = min.z; nearPoint.z = max.z;}

   if ((D3DXPlaneDotCoord(&plane, &nearPoint)) > 0.0f)
   {
      // the box is in front
      return 1;
   }
   if ((D3DXPlaneDotCoord(&plane, &farPoint)) >= 0.0f)
   {
      return 0;
   }
   else
   {
      // the box is behind
      return -1;
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
   ASSERT(false, "AABoundingBox::testCollision(const Triangle&) - Method not implemented");
   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool AABoundingBox::hasVolume() const
{
   return (bool)(min != max);
}

///////////////////////////////////////////////////////////////////////////////

void AABoundingBox::include(const D3DXVECTOR3& pt)
{
   for (char i = 0; i < 3; ++i)
   {
      if      (pt[i] < min[i]) {min[i] = pt[i];}
      if      (pt[i] > max[i]) {max[i] = pt[i];}
   }
}

///////////////////////////////////////////////////////////////////////////////