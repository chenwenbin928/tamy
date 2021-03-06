/// @file   ml-Collada/ColladaScene.h
/// @brief  a Blender scene importer

#ifndef _BLENDER_SCENE_H
#define _BLENDER_SCENE_H

#include "core/FilePath.h"
#include "core/ResourceImporter.h"
#include <string>
#include <vector>
#include <map>


///////////////////////////////////////////////////////////////////////////////

class Model;
class Filesystem;
class ResourcesManager;
class TiXmlDocument;
class TiXmlNode;
class TiXmlElement;
class IProgressObserver;
class SliceDefinition;
class IColladaSlice;
class ISceneSlice;
class Entity;

///////////////////////////////////////////////////////////////////////////////

class ColladaScene : public TResourceImporter< Model >
{
private:
   TiXmlDocument*                                  m_document;
   FilePath                                        m_deploymentDir;

   typedef std::vector< SliceDefinition* >         SlicesDefinitions;
   SlicesDefinitions                               m_definitions;

   typedef std::map< std::string, IColladaSlice* > SlicesMap; 
   SlicesMap                                       m_slicesMap;

   typedef std::vector< ISceneSlice* >             SceneSlicesMap;
   SceneSlicesMap                                  m_sceneSlicesMap;

public:
   /**
    * Constructor.
    */
   ColladaScene( const FilePath& path, ResourcesManager& rm, IProgressObserver* observer );
   ~ColladaScene();

   /**
    * Returns the used instance of progress observer.
    */
   inline IProgressObserver& getObserver() { return *m_observer; }

   /**
    * Returns the used instance of resources manager.
    */
   inline ResourcesManager& getResourcesManager() { return m_rm; }

   /**
    * Returns path to the directory all imported elements should be deployed in.
    */
   inline const FilePath& getDeploymentDir() const { return m_deploymentDir; }

   // -------------------------------------------------------------------------
   // ResourceImporter implementation
   // -------------------------------------------------------------------------
   void import( Model& scene );

   // -------------------------------------------------------------------------
   // Slices management
   // -------------------------------------------------------------------------

   /**
    * Adds a new scene slice in the process of slice parsing.
    *
    * @param id       slice id
    * @param slice
    */
   void addSceneSlice( ISceneSlice* slice );

   /**
    * Adds a new slice in the process of slice parsing.
    *
    * @param id       slice id
    * @param slice
    */
   void addSlice( const std::string& id, IColladaSlice* slice );

   /**
    * Returns a reference to the required resource instance, if one exists.
    *
    * @param id       resource id
    */
   template < typename T >
   T& getResource( const std::string& id ) const;

   /**
    * Returns a new instance of the required entity, if one exists.
    *
    * @param id       entity id
    */
   Entity* getEntity( const std::string& id ) const;
};

///////////////////////////////////////////////////////////////////////////////

class SliceDefinition
{
public:
   virtual ~SliceDefinition() {}

   virtual void parse( TiXmlNode* sceneSlice, ColladaScene& host ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

template < typename T >
class TSliceDefinition : public SliceDefinition
{
private:
   std::string    m_sliceTag;
   std::string    m_sectionTag;

public:
   TSliceDefinition( const std::string& sliceTag, const std::string& sectionTag );

   // -------------------------------------------------------------------------
   // SliceDefinition implementation
   // -------------------------------------------------------------------------
   void parse( TiXmlNode* sceneSlice, ColladaScene& host );
};

///////////////////////////////////////////////////////////////////////////////

#include "ml-Collada/ColladaScene.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _BLENDER_SCENE_H
