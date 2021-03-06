/// @file   TamyEditor/RPBVec4.h
/// @brief  rendering pipeline block representing a vector provider node
#pragma once

#include "core-Renderer/RPVec4Node.h"
#include "GraphBlock.h"
#include "core/ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Rendering pipeline block representing a vector provider node.
 */
class RPBVec4 : public TGraphBlock< RPVec4Node, RenderingPipelineNode >
{
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   RPBVec4() : TGraphBlock< RPVec4Node, RenderingPipelineNode >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   RPBVec4( RPVec4Node& node );
};

///////////////////////////////////////////////////////////////////////////////
