/// @file   core-Renderer/MNCamera.h
/// @brief  a node that provides information about the active camera
#pragma once

#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

class MSFloatOutput;
class MSMatrixOutput;

///////////////////////////////////////////////////////////////////////////////

class MNCamera : public MaterialNode
{
   DECLARE_ALLOCATOR( MNCamera, AM_ALIGNED_16 );
   DECLARE_CLASS()

private:
   MSFloatOutput*    m_nearZ;
   MSFloatOutput*    m_farZ;
   MSFloatOutput*    m_viewportWidth;
   MSFloatOutput*    m_viewportHeight;
   MSMatrixOutput*   m_invView;
   MSMatrixOutput*   m_invProj;

public:
   /**
    * Constructor.
    */
   MNCamera();
   ~MNCamera();

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();

   // -------------------------------------------------------------------------
   // MaterialNode implementation
   // -------------------------------------------------------------------------
   void preRender( Renderer& renderer, const MaterialInstance& instance, const MaterialEntity& entity ) const;
};

///////////////////////////////////////////////////////////////////////////////
