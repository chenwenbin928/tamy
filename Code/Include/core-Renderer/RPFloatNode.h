/// @file   core-Renderer/RPFloatNode.h
/// @brief  a node that provides a float value
#pragma once

#include "core-Renderer/RenderingPipelineNode.h"


///////////////////////////////////////////////////////////////////////////////

class RPFloatOutput;

///////////////////////////////////////////////////////////////////////////////

class RPFloatNode : public RenderingPipelineNode
{
   DECLARE_ALLOCATOR( RPFloatNode, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   // static data
   float             m_value;

   // runtime data
   RPFloatOutput*    m_output;
 
public:
   /**
    * Constructor.
    */
   RPFloatNode();
   ~RPFloatNode();

   // -------------------------------------------------------------------------
   // RenderingPipelineNode implementation
   // -------------------------------------------------------------------------
   void onUpdate( RenderingPipelineMechanism& host ) const;

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////
