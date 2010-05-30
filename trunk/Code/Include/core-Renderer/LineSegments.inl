#ifndef _LINE_SEGMENTS_H
#error "This file can only be included from LineSegments.h"
#else

#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

LineSegment::LineSegment()
: start( 0, 0, 0 )
, end( 0, 0, 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

LineSegment::LineSegment( const D3DXVECTOR3& _start, const D3DXVECTOR3& _end, const Color& _color )
: start( _start )
, end( _end )
, color( _color )
{
}

///////////////////////////////////////////////////////////////////////////////

bool LineSegment::operator==(const LineSegment& rhs) const
{
   return ((D3DXVec3LengthSq(&(start - rhs.start)) < 0.001f) && 
      (D3DXVec3LengthSq(&(end - rhs.end)) < 0.001f));
}

///////////////////////////////////////////////////////////////////////////////

bool LineSegment::operator!=(const LineSegment& rhs) const
{
   return !(*this == rhs);
}

///////////////////////////////////////////////////////////////////////////////

#endif // _LINE_SEGMENTS_H
