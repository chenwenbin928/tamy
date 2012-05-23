/// @file   core-Point.h
/// @brief  a simple point representation
#pragma once

#include <iostream>
#include "core/Vector.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * A simple point representation.
 */
struct Point
{
   int x;
   int y;

   Point( int _x, int _y ) : x(_x), y(_y) {}
   Point() : x(0), y(0) {}
   Point( const Vector& vec ) : x( (int)vec.x ), y( (int)vec.y ) {}

   bool operator==(const Point& rhs) const
   {
      return (x == rhs.x) && (y == rhs.y);
   }

   bool operator!=(const Point& rhs) const
   {
      return !(*this == rhs);
   }

   Point operator-(const Point& rhs) const 
   {
      Point newPt(x - rhs.x, y - rhs.y);
      return newPt;
   }

   Point operator+(const Point& rhs) const 
   {
      Point newPt(x + rhs.x, y + rhs.y);
      return newPt;
   }

   friend std::ostream& operator<<( std::ostream& stream, const Point& rhs )
   {
      stream << "[" << rhs.x << ", " << rhs.y << "]";
      return stream;
   }
};

///////////////////////////////////////////////////////////////////////////////