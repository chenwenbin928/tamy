/// @file   TamyEditor/RPBSceneRender.h
/// @brief  rendering pipeline block representing a node that renders scenes
#pragma once

#include "core-Renderer/RPSceneRenderNode.h"
#include "GraphBlock.h"
#include "core/ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Rendering pipeline block representing a node that renders scenes.
 */
class RPBSceneRender : public TGraphBlock< RPSceneRenderNode, RenderingPipelineNode >
{
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   RPBSceneRender() : TGraphBlock< RPSceneRenderNode, RenderingPipelineNode >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   RPBSceneRender( RPSceneRenderNode& node );
};

///////////////////////////////////////////////////////////////////////////////
