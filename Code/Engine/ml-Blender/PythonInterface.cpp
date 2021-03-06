#include "ml-Blender/PythonInterface.h"
#include "ml-Blender/BlenderSceneExporter.h"
#include "ml-Blender/TamyMaterial.h"
#include "ml-Blender/TamyMesh.h"
#include "ml-Blender/TamyScene.h"
#include "core.h"


///////////////////////////////////////////////////////////////////////////////

void begin_export( const char* filesystemRoot, const char* exportDir, int entitiesCount )
{
   // reset the export tool's contents, so that we start fresh
   BlenderSceneExporter::getInstance().initialize( filesystemRoot, exportDir, entitiesCount );
}

///////////////////////////////////////////////////////////////////////////////

void export_textures( TamyTexture* arrTextures, int texturesCount )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();

   for ( int i = 0; i < texturesCount; ++i )
   {
      exporter.createTexture( arrTextures[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void export_materials( TamyMaterial* arrMaterials, int materialsCount )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();
   exporter.storeMaterials( arrMaterials, materialsCount );
}

///////////////////////////////////////////////////////////////////////////////

void export_meshes( TamyMesh* arrMeshes, int meshesCount )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();

   for ( int i = 0; i < meshesCount; ++i )
   {
      exporter.createMesh( arrMeshes[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void export_geometry_entity( TamyGeometry geometry )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();
   SpatialEntity* entity = exporter.createGeometryEntity( geometry );

   Matrix globalMtx;
   geometry.localMatrix.applyTo( globalMtx );
   exporter.addEntity( entity, globalMtx, geometry.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_light_entity( TamyLight light )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();
   SpatialEntity* entity = exporter.createLightEntity( light );

   Matrix globalMtx;
   light.localMatrix.applyToLight( globalMtx );
   exporter.addEntity( entity, globalMtx, light.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_world_settings( TamyWorld world )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();
   exporter.setupWorld( world );
}


///////////////////////////////////////////////////////////////////////////////

void assemble_scene( const char* sceneName )
{
   BlenderSceneExporter& exporter = BlenderSceneExporter::getInstance();

   exporter.assembleScene( sceneName );
}

///////////////////////////////////////////////////////////////////////////////
