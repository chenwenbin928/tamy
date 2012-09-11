/// @file   TamyEditor/MBBool.h
/// @brief  material pipeline block representing a bool value provider node
#pragma once

#include "core-Renderer/MNBool.h"
#include "PipelineBlock.h"
#include "core/ResourceHandle.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Material pipeline block representing a bool value provider node.
 */
class MBBool : public TPipelineBlock< MNBool, MaterialNode >
{
   DECLARE_CLASS()

public:
   /**
    * Default constructor required by the RTTI system.
    */
   MBBool() : TPipelineBlock< MNBool, MaterialNode >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   MBBool( MNBool& node );
};

///////////////////////////////////////////////////////////////////////////////
