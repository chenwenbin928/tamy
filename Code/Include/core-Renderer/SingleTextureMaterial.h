#pragma once

/// @file   core-Renderer\SingleTextureMaterial.h
/// @brief  a material that can include up to one texture

#include "core-MVC\Entity.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\RenderState.h"
#include <d3dx9.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class PixelShader;
class Texture;

///////////////////////////////////////////////////////////////////////////////

// TODO: setting the renderState in case based on the texture used etc.

/**
 * An atomic object that can be rendered Such an object consists of geometry
 * and effects that render the object in a particular way.
 */
class SingleTextureMaterial : public Entity, public RenderState
{
   DECLARE_CLASS( SingleTextureMaterial )

private:
   PixelShader*         m_shader;
   Material             m_material;
   Texture*             m_texture;

public:
   /**
    * Default constructor.
    */
   SingleTextureMaterial( const std::string& name = "" );
   ~SingleTextureMaterial();

   /**
    * This method allows to set an arbitrary material for the effect to use.
    * 
    * @param material
    */
   inline void setMaterial( const Material& material ) { m_material = material; }

   /**
    * This method allows to set an arbitrary texture for the effect to use.
    * 
    * @param texture
    */
   inline void setTexture( Texture& texture ) { m_texture = &texture; }

   // -------------------------------------------------------------------------
   // RenderState implementation
   // -------------------------------------------------------------------------
   void onPreRender( Renderer& renderer );
   void onPostRender( Renderer& renderer );

protected:
   // -------------------------------------------------------------------------
   // Entity implementation
   // -------------------------------------------------------------------------
   void onAttached( Entity& parent );
   void onDetached( Entity& parent );
   void onComponentAdded( Component< Model >& component );
};

///////////////////////////////////////////////////////////////////////////////
