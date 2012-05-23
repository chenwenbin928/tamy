/// @file   core-Renderer/RPPostProcessNode.h
/// @brief  a rendering pipeline node responsible for applying a post process effect.
#pragma once

#include "core-Renderer/RenderingPipelineNode.h"
#include "core/RuntimeData.h"
#include "core-Renderer/PixelShaderNodeOperator.h"


///////////////////////////////////////////////////////////////////////////////

class RenderTarget;
class Renderer;
class PixelShader;

///////////////////////////////////////////////////////////////////////////////

/**
 * A rendering pipeline node responsible for applying a post process effect.
 */
class RPPostProcessNode : public RenderingPipelineNode
{
   DECLARE_CLASS()

private:
   // static data
   PixelShader*                                                   m_shader;
   std::string                                                    m_renderTargetId;

   // runtime data
   PixelShaderNodeOperator< RenderingPipelineNode >*              m_shaderNode;
   TRuntimeVar< RenderTarget* >                                   m_renderTarget;

public:
   RPPostProcessNode();
   virtual ~RPPostProcessNode();

   // -------------------------------------------------------------------------
   // RenderingPipelineNode implementation
   // -------------------------------------------------------------------------
   void onCreateLayout( RenderingPipelineMechanism& host ) const;
   void onUpdate( RenderingPipelineMechanism& host ) const;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property );
   void onPropertyChanged( ReflectionProperty& property );
   void onObjectLoaded();

protected:
   /**
    * Renders the full screen quad geometry.
    *
    * @param renderer
    * @param rt         render target
    */
   void renderQuad( Renderer& renderer, RenderTarget* rt ) const;
};

///////////////////////////////////////////////////////////////////////////////