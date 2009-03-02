#pragma once

#include <list>
#include <vector>
#include <map>
#include <string>
#include "Face.h"
#include "LitVertex.h"


///////////////////////////////////////////////////////////////////////////////

class Node;
class Camera;
class Light;
class Renderer;
class AbstractGraphicalEntity;
class LightReflectingProperties;
class Texture;
class Material;
struct Color;
class Managable;
class SkyBox;
class GraphicalEntityLoader;
class FileGraphicalEntityLoader;
struct MeshDefinition;
struct AnimationDefinition;
class Skeleton;
class GraphicalEntity;
class SkinnedGraphicalEntity;
struct SkinBoneDefinition;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is responsible for managing the instances of objects
 * we use to populate our scene - creating and deleting them, 
 * instanciating them if applicable etc.
 */
class ResourceManager
{
private:
   std::list<Managable*> m_allObjects;
   std::map<std::string, AbstractGraphicalEntity*> m_graphicalEntities;
   std::vector<Material*> m_materials;
   std::list<LightReflectingProperties*> m_lightReflectingProperties;
   std::map<std::string, Texture*> m_textures;
   std::string m_texturesDirPath;

   std::list<FileGraphicalEntityLoader*> m_graphicalEntitiesLoaders;

public:
   ResourceManager(const std::string& texturesDirPath);

   virtual ~ResourceManager();

   /**
    * This method returns an instance of the render consistent
    * with other entities implementations.
    */
   virtual Renderer& getRendererInstance() = 0;

   // --------------------------- Graphical Entities --------------------------

   /**
    * Checks whether a graphical object with a specified
    * name already exists
    */
   bool isGraphicalEntityRegistered(const std::string& name) const;

   /**
    * This method can be used to register an entity with the system
    * for further management
    *
    * @throws std::logic_error if an entity with the specified name already exists
    */
    void registerGraphicalEntity(const std::string& name,
                                 AbstractGraphicalEntity* entity);

   /**
    * Loads a mesh using a graphical entity loader
    *
    * @throws std::logic_error if an entity with the specified name already exists
    * @param name - name of the file from which the mesh should be loaded.
    *               It's also the name under which the mesh will be registered.
    */
   AbstractGraphicalEntity& loadGraphicalEntity(const std::string& name, GraphicalEntityLoader& loader);

   /**
    * Rturns a reference to a graphical entity providing one exists
    *
    * @throws std::out_of_range exception if one hasn't been registered yet
    */
   AbstractGraphicalEntity& getGraphicalEntity(const std::string& name);

   /**
    * Returns a loader capable of dealing with a specified file type,
    * providing one exists.
    *
    * @throws std::out_of_range if there's no loader that's capable
    *                           of dealing with the specified file type
    */
   GraphicalEntityLoader& getLoaderForFile(const std::string& fileName);

   /**
    * This method should return a valid implementation of the GraphicalEntity class
    */
   virtual GraphicalEntity* createGraphicalEntity(const std::string& name,
                          const MeshDefinition& subMesh,
                          const std::vector<Material*>& registeredMaterials) = 0;

   /**
    * This method should return a valid implementation of the GraphicalEntity class
    */
   virtual SkinnedGraphicalEntity* createSkinedGraphicalEntity(const std::string& name,
                          const MeshDefinition& subMesh,
                          const std::vector<Material*>& registeredMaterials) = 0;

   // -------------------------------- Cameras --------------------------------

   Camera* createCamera(const std::string& name);

   // --------------------------------- Lights ---------------------------------

   Light* createLight(const std::string& name);

   // ------------------------------- Materials --------------------------------

   /**
    * The method checks if particular set of light reflecting properties is registered
    */
   bool areLightReflectingPropertiesRegistered(const LightReflectingProperties& lrp) const;

   /**
    * Retrieves the light reflecting properties matching those 
    * of the sample passed as an argument
    */
   LightReflectingProperties& getLightReflectingProperties(const LightReflectingProperties& lrp) const;

   /**
    * Registers new set of light reflecting properties
    */
   LightReflectingProperties& addLightReflectingProperties(LightReflectingProperties* lrp);

   /**
    * Checks whether a texture with a certain name has been already created
    */
   bool isTextureRegistered(const std::string& name) const;

   /**
    * This method loads a texture with a given name from a file.
    * The name of the file should match that of the texture name specified.
    *
    * @throws std::logic_error is a texture with the specified name already exists
    */
   Texture& loadTexture(const std::string& name);

   /**
    * This method retrieves a pointer to a texture with a specified name.
    *
    * @throws std::out_of_range exception if one hasn't been registered yet
    */
   Texture& getTexture(const std::string& name) const;

   /**
    * Returns a material with the give id, if one exists.
    * @throws std::out_of_range exception if one hasn't been defined
    */
   Material& getMaterial(unsigned int id);

   /**
    * This method adds a new material and returns an ID assigned to it.
    * If for some mreason an alike material exists, its ID is returned
    * and the collection of materials is not expanded
    */
   unsigned int addMaterial(const std::string& textureName, 
                            const Color& ambient,
                            const Color& diffuse,
                            const Color& specular,
                            const Color& emissive,
                            float power);

   // ------------------------------ Decorations ------------------------------

   /**
    * This method creates a skybox instance
    */
   SkyBox& createSkyBox();

protected:
   /**
    * This method should return a valid implementation of the Light class
    */
   virtual Light* createLightImpl(const std::string& name) = 0;

   virtual LightReflectingProperties* createLightReflectingProperties() = 0;

   virtual Texture* createEmptyTexture() = 0;

   virtual Texture* loadTexture(const std::string& textureDirPath, const std::string& name) = 0;

   virtual SkyBox* createSkyBoxImpl() = 0;

   /**
    * This method allows the implementations to register
    * the mesh file handlers specific for a particular implementation type
    */
   void registerFileGraphicalEntityLoader(FileGraphicalEntityLoader* loader);

private:
   Texture& getEmptyTexture();
};

///////////////////////////////////////////////////////////////////////////////
