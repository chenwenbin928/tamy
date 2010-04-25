#ifndef _SHADER_EFFECT_H
#define _SHADER_EFFECT_H

/// @file   core-Renderer\ShaderEffect.h
/// @brief  an effect based on a shader

#include "core-Renderer\Effect.h"
#include "core-Renderer\Shader.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This abstract class is a base for all effects based on shaders.
 * Its instances can be automatically created using the EffectResource
 * class.
 */
class ShaderEffect : public Effect
{
private:
   Shader* m_shader;

public:
   virtual ~ShaderEffect();

protected:
   /**
    * Constructor.
    */
   ShaderEffect();

   /**
    * Method called by the EffectResource that creates
    * an instance of this class. It initializes the effect
    * with a shader instance.
    *
    * @param shader  shader program the effect will be using
    */
   void initialize(Shader& shader); 

   /**
    * Gives access to the shader program instance.
    */
   inline Shader& shader();
   friend class EffectResource;

   // -------------------------------------------------------------------------
   // Effect implementation
   // -------------------------------------------------------------------------
   unsigned int beginRendering();
   void beginPass(unsigned int passIdx);
   void endPass(unsigned int passIdx);
   void endRendering();

   // -------------------------------------------------------------------------
   // Effect hooks
   // -------------------------------------------------------------------------
   /**
    * Called before the rendering starts.
    */
   virtual void onBeginRendering() {}

   /**
    * Called before the specified rendering pass starts.
    */
   virtual void onBeginPass(unsigned int passIdx) {}

   /**
    * Called after the specified rendering pass ends.
    */
   virtual void onEndPass(unsigned int passIdx) {}
   
   /**
    * Called after the rendering ends.
    */
   virtual void onEndRendering() {}
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer\ShaderEffect.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _SHADER_EFFECT_H