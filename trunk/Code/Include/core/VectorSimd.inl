#ifndef _VECTOR_H
#error "This file can only be included from Vector.h"
#else


///////////////////////////////////////////////////////////////////////////////

float& Vector::operator[]( int idx )
{
   return v.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

float Vector::operator[]( int idx ) const
{
   return v.v[idx];
}

///////////////////////////////////////////////////////////////////////////////

void Vector::store( QuadStorage& storage ) const
{
   const ALIGN_16 float p[] = { x, y, z, w };
   storage = _mm_load_ps( p );
}

///////////////////////////////////////////////////////////////////////////////

void Vector::load( const QuadStorage& storage )
{
   ALIGN_16 float p[4];
   _mm_store_ps( (float*)p, storage );

   x = p[0];
   y = p[1];
   z = p[2];
   w = p[3];
}

///////////////////////////////////////////////////////////////////////////////

#endif // _VECTOR_H
