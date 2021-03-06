#include "ml-IWF\IWFMeshLoader.h"
#include "ml-IWF\libIWF\iwfObjects.h"
#include "ml-IWF\MeshDefinition.h"
#include "core\TriangleUtil.h"


///////////////////////////////////////////////////////////////////////////////

IWFMeshLoader::IWFMeshLoader(iwfMesh* fileMesh, 
                             const std::vector<iwfTexture*>& textures, 
                             const std::vector<iwfMaterial*>& materials)
      : m_fileMesh(fileMesh),
      m_textures(textures),
      m_materials(materials)
{
}

///////////////////////////////////////////////////////////////////////////////

void IWFMeshLoader::parseMesh( Array<MeshDefinition>& meshes, const std::string& name )
{
   Vector minPos; minPos.set(10000000, 10000000, 10000000);
   Vector maxPos; maxPos.set(-10000000, -10000000, -10000000);

   std::vector<MaterialDefinition> tmpMaterials;
   MeshDefinition* mesh = NULL;

   // let's parse the mesh and explode it into a group of engine laws abiding meshes
   for (ULONG surfaceIdx = 0; surfaceIdx < m_fileMesh->SurfaceCount; ++surfaceIdx)
   {
      iwfSurface* surface = m_fileMesh->Surfaces[surfaceIdx];

      // determine the indices of the material and a texture as we loaded them into our structures
      iwfTexture* tex = NULL;
      iwfMaterial* mat = NULL;
      bool usesMaterial = ((surface->Components & SCOMPONENT_MATERIALS) == SCOMPONENT_MATERIALS);
      bool usesTexture = ((surface->Components & SCOMPONENT_TEXTURES) == SCOMPONENT_TEXTURES);
      if (usesMaterial && (surface->ChannelCount > 0) && (surface->MaterialIndices[0] >= 0))
      {
         mat = m_materials.at(surface->MaterialIndices[0]);
      }
      if (usesTexture && (surface->ChannelCount > 0) && (surface->TextureIndices[0] >= 0))
      {
         tex = m_textures.at(surface->TextureIndices[0]);
      }

      // select proper mesh based on the material
      mesh = selectMeshByMaterial( tex, mat, meshes );

      // geometry creation step 1: create faces
      addSurface(surface, mesh->vertices.size(), mesh->faces);

      // geometry creation step 2: create vertices
      for (UINT i = 0; i < surface->VertexCount; i++)
      {
         const iwfVertex& vtx = surface->Vertices[i];
         Vector vertexPos; vertexPos.set(vtx.x, vtx.y, vtx.z );
         Vector vertexNormal; vertexNormal.set(vtx.Normal.x, vtx.Normal.y, vtx.Normal.z );

         mesh->vertices.push_back( LitVertex() );
         LitVertex& vertex = mesh->vertices.back();

         vertexPos.store( vertex.m_coords );
         vertexNormal.store( vertex.m_normal );
         vertex.m_textureCoords[0] = vtx.TexCoords[0][0];
         vertex.m_textureCoords[1] = vtx.TexCoords[0][1];

         minPos.setMin( minPos, vertexPos );
         maxPos.setMax( maxPos, vertexPos );
      }
   }

   // translate all vertices so that the mesh is centered around the global origin
   Vector meshTranslation;
   meshTranslation.setAdd( minPos, maxPos );
   meshTranslation.mul( Float_Inv2 );
   
   unsigned int meshesCount = meshes.size();
   for (unsigned int i = 0; i < meshesCount; ++i)
   {
      mesh = &(meshes[i]);
      for ( std::vector<LitVertex>::iterator vertexIt = mesh->vertices.begin(); vertexIt != mesh->vertices.end(); ++vertexIt )
      {
         vertexIt->m_coords.v[0] -= meshTranslation[0]; 
         vertexIt->m_coords.v[1] -= meshTranslation[1]; 
         vertexIt->m_coords.v[2] -= meshTranslation[2]; 
      }

      mesh->localMtx.setTranslation( meshTranslation );
   }


   // set meshes names
   if ( meshesCount == 1 )
   {
      // set the mesh's name
      meshes[0].name = name;
   }
   else
   {
      char tmpName[512];
      for (unsigned int i = 0; i < meshesCount; ++i)
      {
         sprintf_s( tmpName, "%s_%d", name.c_str(), i );
         meshes[i].name = tmpName;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void IWFMeshLoader::addSurface(iwfSurface* surface, 
                               USHORT vertexStart, 
                               std::vector<Face >& faces)
{
   if (surface->IndexCount > 0)
   {
      ULONG indexType = surface->IndexFlags & INDICES_MASK_TYPE;

      // Interpret indices (we want them in tri-list format)
      switch (indexType)
      {
      case INDICES_TRILIST:
         {
            ULONG numFaces = surface->IndexCount / 3;
            for (ULONG i = 0; i < numFaces; i++) 
            {
               ULONG startIdx = i * 3;
               faces.push_back(Face((USHORT)surface->Indices[startIdx] + vertexStart,
                                            (USHORT)surface->Indices[startIdx + 1] + vertexStart,
                                            (USHORT)surface->Indices[startIdx + 2] + vertexStart));
            }
            break;
         }

      case INDICES_TRISTRIP:
         {
            // Index in strip order
            for (long i = 0; i < surface->IndexCount - 2; i++)
            {
               // Starting with triangle 0.
               // Is this an 'Odd' or 'Even' triangle
               if ((i % 2) == 0)
               {
                  faces.push_back(Face((USHORT)surface->Indices[i] + vertexStart,
                                               (USHORT)surface->Indices[i + 1] + vertexStart,
                                               (USHORT)surface->Indices[i + 2] + vertexStart));
               } // End if 'Even' triangle
               else
               {
                  faces.push_back(Face((USHORT)surface->Indices[i] + vertexStart,
                                               (USHORT)surface->Indices[i + 2] + vertexStart,
                                               (USHORT)surface->Indices[i + 1] + vertexStart));
               } // End if 'Odd' triangle
            }

            break;
         }

      case INDICES_TRIFAN:
         {
            // Index in fan order.
            for (long i = 1; i < surface->VertexCount - 1; i++)
            {
               faces.push_back(Face((USHORT)surface->Indices[0] + vertexStart,
                                            (USHORT)surface->Indices[i] + vertexStart,
                                            (USHORT)surface->Indices[i + 1] + vertexStart));
            }

            break;
         }
      }
   }
   else
   {
      ULONG vertexType = surface->VertexFlags & VERTICES_MASK_TYPE;

      // Interpret vertices (we want our indices in tri-list format)
      switch (vertexType)
      {
      case VERTICES_TRILIST:
         {
            // Straight fill
            USHORT numFaces = surface->VertexCount / 3;
            for (USHORT i = 0; i < numFaces; i++) 
            {
               USHORT startIdx = i * 3;
               faces.push_back(Face(startIdx + vertexStart,
                                            startIdx + 1 + vertexStart,
                                            startIdx + 2 + vertexStart));
            }
            break;
         }

      case VERTICES_TRISTRIP:

         // Index in strip order
         for (USHORT i = 0; i < surface->VertexCount - 2; i++)
         {
            // Starting with triangle 0.
            // Is this an 'Odd' or 'Even' triangle
            if ( (i % 2) == 0 )
            {
               faces.push_back(Face(i + vertexStart, 
                                             i + 1 + vertexStart, 
                                             i + 2 + vertexStart));
            } // End if 'Even' triangle
            else
            {
               faces.push_back(Face(i + vertexStart, 
                                             i + 2 + vertexStart, 
                                             i + 1 + vertexStart));
            } // End if 'Odd' triangle

         }

         break;

      case VERTICES_TRIFAN:

         // Index in fan order.
         for (USHORT i = 1; i < surface->VertexCount - 1; i++)
         {
            faces.push_back(Face(vertexStart, 
                                          i + vertexStart, 
                                          i + 1 + vertexStart));
         }

         break;

      }
   }
}

///////////////////////////////////////////////////////////////////////////////

MeshDefinition* IWFMeshLoader::selectMeshByMaterial( iwfTexture* texture, iwfMaterial* material, Array< MeshDefinition >& meshes )
{
   std::string newMaterialName = ((material != NULL) && (material->Name != NULL)) ? material->Name : "";
   std::string newTextureName  = ((texture != NULL) && (texture->Name != NULL)) ? texture->Name : "";

   std::string matName = newMaterialName + std::string("_") + newTextureName;
   // look for a similar material first
   for (unsigned int meshIdx = 0; meshIdx < meshes.size(); ++meshIdx)
   {
      MaterialDefinition& mat = meshes.at(meshIdx).material;
      if (mat.matName == matName)
      {
         return &(meshes[meshIdx]);
      }
   }

   // this is something we don't have yet - create a new material
   meshes.push_back(MeshDefinition());
   MaterialDefinition& def = meshes.back().material;
   def.matName = matName;
   if (material != NULL)
   {
      def.ambient.r = material->Ambient.r;
      def.ambient.g = material->Ambient.g;
      def.ambient.b = material->Ambient.b;
      def.ambient.a = material->Ambient.a;

      def.diffuse.r = material->Diffuse.r;
      def.diffuse.g = material->Diffuse.g;
      def.diffuse.b = material->Diffuse.b;
      def.diffuse.a = material->Diffuse.a;

      def.specular.r = material->Specular.r;
      def.specular.g = material->Specular.g;
      def.specular.b = material->Specular.b;
      def.specular.a = material->Specular.a;

      def.emissive.r = material->Emissive.r;
      def.emissive.g = material->Emissive.g;
      def.emissive.b = material->Emissive.b;
      def.emissive.a = material->Emissive.a;

      def.power = material->Power;
   }

   if (texture != NULL)
   {
      def.texName = newTextureName;
   }

   return &(meshes.back());
}

///////////////////////////////////////////////////////////////////////////////
