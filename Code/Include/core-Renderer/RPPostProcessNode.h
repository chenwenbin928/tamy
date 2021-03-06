/// @file   core-Renderer/RPPostProcessNode.h
/// @brief  a rendering pipeline node responsible for applying a post process effect.
#pragma once

#include "core-Renderer/RenderingPipelineNode.h"
#include "core-Renderer/PixelShaderNodeOperator.h"
#include "core/RuntimeData.h"
#include "core/RefPtr.h"


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
   DECLARE_ALLOCATOR( RPPostProcessNode, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   TRefPtr< PixelShader >                                         m_shader;
   std::string                                                    m_renderTargetId;

   // runtime data
   PixelShaderNodeOperator< RenderingPipelineNode >*              m_shaderNode;
   TRuntimeVar< Array< RenderTarget* >* >                         m_renderTargets;

public:
   RPPostProcessNode();
   virtual ~RPPostProcessNode();

   // -------------------------------------------------------------------------
   // RenderingPipelineNode implementation
   // -------------------------------------------------------------------------
   void onGraphLoaded();
   void onCreateLayout( RenderingPipelineMechanism& host ) const;
   void onUpdate( RenderingPipelineMechanism& host ) const;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPrePropertyChanged( ReflectionProperty& property );
   void onPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////
