#pragma once

/// @file   core-Renderer\SingleTextureEffect.h
/// @brief  simple rendering effect

#include "core-Renderer\ShaderEffect.h"
#include "core-Renderer\Material.h"


///////////////////////////////////////////////////////////////////////////////

class Shader;
class Renderable;
class Material;
class Texture;
class Camera;
class ResourcesManager;

///////////////////////////////////////////////////////////////////////////////

/**
* This effect will render the geometry using a single texture
* and a material definition.
*/
class SingleTextureEffect : public ShaderEffect
{
   DECLARE_CLASS(SingleTextureEffect)

private:
   Renderable* m_renderable;
   Camera* m_camera;
   Material m_material;
   Texture* m_texture;

public:
   /**
    * Constructor (default.
    */
   SingleTextureEffect();

   /**
    * This method allows to set an arbitrary material for the effect to use.
    * 
    * @param material
    */
   void setMaterial(const Material& material);

   /**
    * This method allows to set an arbitrary texture for the effect to use.
    * 
    * @param texture
    */
   void setTexture(Texture& texture);

protected:
   // -------------------------------------------------------------------------
   // Entity implementation 
   // -------------------------------------------------------------------------
   void onAttached(Entity& parent);
   void onDetached(Entity& parent);
   void onAttached(Model& hostModel);
   void onDetached(Model& hostModel);

   // -------------------------------------------------------------------------
   // ShaderEffect implementation 
   // -------------------------------------------------------------------------
   void onBeginRendering();
};

///////////////////////////////////////////////////////////////////////////////
