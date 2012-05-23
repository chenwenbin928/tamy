/// @file   core-Renderer\LitVertex.h
/// @brief  geometry vertex definition
#pragma once

#include "core\Vector.h"
#include "core\types.h"
#include <iostream>


///////////////////////////////////////////////////////////////////////////////

class InStream;
class OutStream;

///////////////////////////////////////////////////////////////////////////////

/**
 * Geometry vertex definition.
 */
struct LitVertex
{
   TVector<3>   m_coords;
   TVector<3>   m_normal;
   TVector<2>   m_textureCoords;

   static ulong FVF;

   /**
    * Constructor for a simple mesh vertex.
    */
   LitVertex( float x, float y, float z, float nx, float ny, float nz, float u, float v )
   {
      m_coords.v[0] = x; m_coords.v[1] = y; m_coords.v[2] = z;
      m_normal.v[0] = nx; m_normal.v[1] = ny; m_normal.v[2] = nz;
      m_textureCoords.v[0] = u; m_textureCoords.v[1] = v;
   }

   LitVertex()
   {
      m_coords.setUniform( 0.0f );
      m_normal.setUniform( 0.0f );
      m_textureCoords.setUniform( 0.0f );
   }

   friend OutStream& operator<<( OutStream& stream, const LitVertex& vtx );
   friend InStream& operator>>( InStream& stream, LitVertex& vtx );
};

///////////////////////////////////////////////////////////////////////////////

struct VertexWeight
{
   TVector<4>          m_weights;
   TVector<4>          m_indices;

   static ulong FVF;

   VertexWeight()
   {
      m_weights.setUniform( 0.0f );
      m_weights.setUniform( -1.0f );
   }
   
   friend OutStream& operator<<( OutStream& stream, const VertexWeight& weight );
   friend InStream& operator>>( InStream& stream, VertexWeight& weight );
};

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& stream, const LitVertex& vertex );
std::ostream& operator<<( std::ostream& stream, const VertexWeight& vertex );

///////////////////////////////////////////////////////////////////////////////