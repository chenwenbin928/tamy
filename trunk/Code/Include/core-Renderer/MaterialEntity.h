/// @file   core-Renderer/MaterialEntity.h
/// @brief  entity that allows to embed a material in a scene graph
#pragma once

#include "core-MVC/Entity.h"
#include "core-Renderer/RenderState.h"
#include "core-Renderer/SurfaceProperties.h"
#include "core/ReflectionEnum.h"
#include "core/Observer.h"


///////////////////////////////////////////////////////////////////////////////

class Material;
class RuntimeData;
class MaterialNode;
class Texture;
enum GraphBuilderOperation;
enum GraphBuilderNodeOperation;

///////////////////////////////////////////////////////////////////////////////

enum MaterialTextures
{
   MT_DIFFUSE_1,
   MT_DIFFUSE_2,
   MT_NORMALS,

   MT_MAX
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Entity that allows to embed a material in a scene graph.
 */
class MaterialEntity :  public Entity, public TRenderState< MaterialEntity >, 
                        public Observer< Material, GraphBuilderOperation >,
                        public Observer< MaterialNode, GraphBuilderNodeOperation >
{
   DECLARE_CLASS()

private:
   // static data
   SurfaceProperties                m_surfaceProperties;
   Texture*                         m_texture[ MT_MAX ];
   Material*                        m_material;

   // runtime data
   RuntimeDataBuffer*               m_dataBuf;
   std::vector< MaterialNode* >     m_nodesQueue;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   MaterialEntity( const std::string& name = "" );
   ~MaterialEntity();

   /**
    * Returns the runtime data buffer.
    */
   inline RuntimeDataBuffer& data() const { return *m_dataBuf; }

   /**
    * Gives access to material instance's surface properties.
    */
   inline SurfaceProperties& accessSurfaceProperties() { return m_surfaceProperties; }

   /**
    * Returns material instance's surface properties.
    */
   inline const SurfaceProperties& getSurfaceProperties() const { return m_surfaceProperties; }

   /**
    * Gives access to the specified texture associated with this material instance.
    *
    * @param textureUsage
    */
   inline Texture* getTexture( MaterialTextures textureUsage ) const { return m_texture[textureUsage]; }

   /**
    * Sets a new texture of the specified usage on the material instance.
    *
    * @param textureUsage
    * @param texture
    */
   inline void setTexture( MaterialTextures textureUsage, Texture* texture ) { m_texture[textureUsage] = texture; }

   /**
    * Sets a new material resource this instance should use to render stuff.
    *
    * @param material
    */
   void setMaterial( Material* material );

   // -------------------------------------------------------------------------
   // RenderState implementation
   // -------------------------------------------------------------------------
   void onPreRender( Renderer& renderer ) const;
   void onPostRender( Renderer& renderer ) const;
   bool onEquals( const MaterialEntity& rhs ) const;
   bool onLess( const MaterialEntity& rhs ) const;

   // -------------------------------------------------------------------------
   // Observer implementation
   // -------------------------------------------------------------------------
   void update( Material& subject );
   void update( Material& subject, const GraphBuilderOperation& msg );
   void update( MaterialNode& subject );
   void update( MaterialNode& subject, const GraphBuilderNodeOperation& msg );

protected:
   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
   void onPrePropertyChanged( ReflectionProperty& property );
   void onPropertyChanged( ReflectionProperty& property );

   // -------------------------------------------------------------------------
   // Entity implementation
   // -------------------------------------------------------------------------
   Entity* cloneSelf() const;

private:
   void initializeMaterial();
   void deinitializeMaterial();
   void attachListeners();
   void detachListeners();
};

///////////////////////////////////////////////////////////////////////////////
