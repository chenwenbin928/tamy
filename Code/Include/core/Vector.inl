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

#endif // _VECTOR_H
