#include "core\Vector.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RTTI(Vector)
END_RTTI

///////////////////////////////////////////////////////////////////////////////

Vector::Vector() 
: x(0)
, y(0)
, z(0) 
{}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector(float _x, float _y, float _z) 
: x(_x)
, y(_y)
, z(_z) 
{}

///////////////////////////////////////////////////////////////////////////////

Vector::Vector(const D3DXVECTOR3& dxVec) 
: x(dxVec.x)
, y(dxVec.y)
, z(dxVec.z) 
{}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator==(const Vector& rhs) const
{
   return x == rhs.x && y == rhs.y && z == rhs.z;
}

///////////////////////////////////////////////////////////////////////////////

bool Vector::operator!=(const Vector& rhs) const
{
   return !(*this == rhs);
}

///////////////////////////////////////////////////////////////////////////////

Vector& Vector::operator=(const Vector& rhs)
{
   x = rhs.x;
   y = rhs.y;
   z = rhs.z;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::operator-() const
{
   return Vector(-x, -y, -z);
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::operator+(const Vector& rhs) const
{
   return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
}

///////////////////////////////////////////////////////////////////////////////

Vector& Vector::operator+=(const Vector& rhs)
{
   x += rhs.x;
   y += rhs.y;
   z += rhs.z;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::operator-(const Vector& rhs) const
{
   return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
}

///////////////////////////////////////////////////////////////////////////////

Vector& Vector::operator-=(const Vector& rhs)
{
   x -= rhs.x;
   y -= rhs.y;
   z -= rhs.z;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::operator*(float val) const
{
   return Vector(x * val, y * val, z * val);
}

///////////////////////////////////////////////////////////////////////////////

Vector& Vector::operator*=(float val)
{
   x *= val;
   y *= val;
   z *= val;

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

float Vector::dot(const Vector& rhs) const
{
   return x * rhs.x + y * rhs.y + z * rhs.z;
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::cross(const Vector& rhs) const
{
   return Vector(y * rhs.z - z * rhs.y,
                 z * rhs.x - x * rhs.z,
                 x * rhs.y - y * rhs.x);
}

///////////////////////////////////////////////////////////////////////////////

Vector Vector::normalized() const
{
   float len = sqrt(x * x + y * y + z * z);
   if (len == 0)
   {
      return Vector(0, 0, 0);
   }
   else
   {
      return Vector(x / len, y / len, z / len);
   }
}

///////////////////////////////////////////////////////////////////////////////

Vector& Vector::normalize()
{
   float len = sqrt(x * x + y * y + z * z);
   if (len == 0)
   {
      x = y = z = 0;
   }
   else
   {
      x /= len;
      y /= len;
      z /= len;
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

float Vector::length() const
{
   return sqrt(x * x + y * y + z * z);
}

///////////////////////////////////////////////////////////////////////////////

float Vector::lengthSq() const
{
   return x * x + y * y + z * z;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& stream, const Vector& rhs)
{
   stream << "[" << rhs.x << ", " << rhs.y << ", " << rhs.z << "]";
   return stream;
}

///////////////////////////////////////////////////////////////////////////////