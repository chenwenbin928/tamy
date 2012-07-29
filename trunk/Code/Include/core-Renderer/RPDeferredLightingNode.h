/// @file   core-Renderer/RPDeferredLightingNode.h
/// @brief  a node that provides information about the scene lights
#pragma once

#include "core-Renderer/RenderingPipelineMechanism.h"
#include "core-Renderer/RenderingPipelineNode.h"


///////////////////////////////////////////////////////////////////////////////

class RenderTarget;
class RPTextureInput;
class RPTextureOutput;

///////////////////////////////////////////////////////////////////////////////

/**
 * A node that provides information about the scene lights visible from the active camera.
 */
class RPDeferredLightingNode : public RenderingPipelineNode
{
   DECLARE_CLASS()

private:
   std::string                               m_renderTargetId;

   // runtime data
   TRuntimeVar< Array< RenderTarget* >* >    m_renderTargets;

   // sockets data
   RPTextureInput*                           m_sceneColorInput;
   RPTextureInput*                           m_depthNormalsInput;


public:
   /**
    * Constructor.
    */
   RPDeferredLightingNode();
   ~RPDeferredLightingNode();

   // -------------------------------------------------------------------------
   // RenderingPipelineNode implementation
   // -------------------------------------------------------------------------
   void onCreateLayout( RenderingPipelineMechanism& host ) const;
   void onDestroyLayout( RenderingPipelineMechanism& host ) const;
   void onUpdate( RenderingPipelineMechanism& host ) const;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
   void onPropertyChanged( ReflectionProperty& property );
};

///////////////////////////////////////////////////////////////////////////////
