#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\MeshUtils.h"
#include "core.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE_NO_PARENT( TriangleMesh, ttm, AM_BINARY );
   PARENT( GeometryResource );
   PROPERTY( std::vector<LitVertex>, m_vertices );
   PROPERTY( std::vector<Face>, m_faces );
END_RESOURCE();

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::TriangleMesh()
   : m_boundsDirty( true )
{
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::TriangleMesh( const FilePath& resourceName, const std::vector<LitVertex>& vertices, const std::vector<Face>& faces )
   : GeometryResource( resourceName )
   , m_vertices(vertices)
   , m_faces(faces)
   , m_boundsDirty( false )
{
   calculateBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh::~TriangleMesh()
{

}

///////////////////////////////////////////////////////////////////////////////

const BoundingVolume& TriangleMesh::getBoundingVolume()
{
   if ( m_boundsDirty ) 
   {
      calculateBoundingVolume();
   }

   return m_boundingVol;
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::calculateBoundingVolume()
{
   unsigned int verticesCount = m_vertices.size();
   Vector tmpVec;
   for (unsigned int i = 0; i < verticesCount; ++i)
   {
      tmpVec.load( m_vertices[i].m_coords );
      m_boundingVol.include( tmpVec );
   }

   m_boundsDirty = false;
}

///////////////////////////////////////////////////////////////////////////////

VertexArray* TriangleMesh::getGenericVertexArray() const
{
   TVertexArray<LitVertex>* array = new TVertexArray<LitVertex>();

   unsigned int vtxCount = m_vertices.size();
   for (unsigned int i = 0; i < vtxCount; ++i)
   {
      array->add( m_vertices[i] );
   }

   return array;

}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::render( Renderer& renderer )
{
   new ( renderer() ) RCRenderTriangleMesh( *this );
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::calculateTangents()
{
   MeshUtils::calculateVertexTangents( m_faces, m_vertices );
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::setVertices( const LitVertex* arrVertices, uint verticesCount )
{
   m_vertices.clear();
   m_vertices.resize( verticesCount );
   m_boundingVol.reset();

   Vector point;
   for ( uint i = 0; i < verticesCount; ++i )
   {
      m_vertices[i] = arrVertices[i];

      // recalculate the bounding box
      point.load( m_vertices[i].m_coords );
      m_boundingVol.include( point );
   }

   // mark the render resource as dirty
   m_boundsDirty = true;
   setDirty();
}

///////////////////////////////////////////////////////////////////////////////

void TriangleMesh::setFaces( const Face* arrFaces, uint facesCount )
{
   m_faces.clear();
   m_faces.resize( facesCount );

   for ( uint i = 0; i < facesCount; ++i )
   {
      m_faces[i] = arrFaces[i];
   }

   // mark the render resource as dirty
   setDirty();
}

///////////////////////////////////////////////////////////////////////////////
