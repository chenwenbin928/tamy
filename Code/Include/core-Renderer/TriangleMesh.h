/// @file   core-Renderer\TriangleMesh.h
/// @brief  geometry of a mesh composed of triangles
#ifndef _TRIANGLE_MESH_H
#define _TRIANGLE_MESH_H


#include <vector>
#include "core\Resource.h"
#include "core-Renderer\GeometryResource.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\VertexArray.h"
#include "core-Renderer\Face.h"
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\RenderResource.h"
#include "core\AABoundingBox.h"
#include "core\Matrix.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This is an abstract representation of a mesh geometry composed of
 * triangles.
 * Because such a mesh will preferably be stored in the graphics card's
 * memory for speed reasons, and this can only be achieved by using 
 * some low level 3D lib (DirectX, OpenGL), thus we'll put
 * this responsibility on the TriangleMeshImpl.
 */
class TriangleMesh : public GeometryResource, public RenderResource
{
   DECLARE_ALLOCATOR( TriangleMesh, AM_ALIGNED_16 );
   DECLARE_RESOURCE()

private:
   std::vector< LitVertex >      m_vertices;
   std::vector< Face >           m_faces;

   bool                          m_boundsDirty;
   AABoundingBox                 m_boundingVol;

public:
   /**
    * Default constructor.
    */
   TriangleMesh();

   /**
    * Constructor.
    *
    * @param resourceName
    * @param vertices   mesh vertices
    * @param faces      mesh faces
    */
   TriangleMesh( const FilePath& resourceName, const std::vector<LitVertex>& vertices, const std::vector<Face>& faces );
   ~TriangleMesh();

   /**
    * Returns an array of vertices of the mesh (in their generic form.)
    *
    * @return  model vertices.
    */
   VertexArray* getGenericVertexArray() const;

   /**
    * Returns the faces of the model.
    *
    * @return  model faces
    */
   inline const std::vector<Face>& getFaces() const;

   /**
    * Calculates texture-space tangent vectors. 
    */
   void calculateTangents();

   /**
    * Sets new vertices on the mesh.
    *
    * @param arrVertices
    * @param verticesCount
    */
   void setVertices( const LitVertex* arrVertices, uint verticesCount );

   /**
    * Sets new faces on the mesh.
    *
    * @param arrVertices
    * @param verticesCount
    */
   void setFaces( const Face* arrFaces, uint facesCount );

   // -------------------------------------------------------------------------
   // GeometryResource implementation
   // -------------------------------------------------------------------------
   const BoundingVolume& getBoundingVolume();
   void render( Renderer& renderer );

private:
   /**
    * Recalculates the bounding volume.
    */
   void calculateBoundingVolume();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Triangle mesh rendering command.
 */
class RCRenderTriangleMesh : public RenderCommand
{
   DECLARE_ALLOCATOR( RCRenderTriangleMesh, AM_DEFAULT );

private:
   TriangleMesh&        m_mesh;

public:
   RCRenderTriangleMesh( TriangleMesh& mesh ) : m_mesh( mesh ) { m_mesh.addReference(); }
   ~RCRenderTriangleMesh() { m_mesh.removeReference(); }

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\TriangleMesh.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _TRIANGLE_MESH_H
