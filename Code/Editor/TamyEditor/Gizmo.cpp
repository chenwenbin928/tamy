#include "Gizmo.h"
#include "core-Renderer.h"
#include "core.h"
#include "core-MVC.h"


///////////////////////////////////////////////////////////////////////////////

Gizmo::Gizmo( SpatialEntity& node )
: m_node( node )
, SIZE( 10.f )
, OX_COLOR( 1, 0, 0, 1 )
, OY_COLOR( 0, 1, 0, 1 )
, OZ_COLOR( 0, 0, 1, 1 )
{
   // set the initial mode
   setMode( GM_TRANSLATION );
}

///////////////////////////////////////////////////////////////////////////////

Gizmo::~Gizmo()
{
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::setMode( Mode mode )
{
   // set the new mode and the related geometry
   m_mode = mode;
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::onDebugRender( IDebugDraw& renderer ) const
{
   switch( m_mode )
   {
   case GM_TRANSLATION:
      {
         drawTranslationGizmo( renderer );
         break;
      }

   case GM_ROTATION:
      {
         drawRotationGizmo( renderer );
         break;
      }

   case GM_SCALING:
      {
         drawScalingGizmo( renderer );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::drawTranslationGizmo( IDebugDraw& renderer ) const
{
   const D3DXMATRIX& mtx = m_node.getGlobalMtx();

   D3DXVECTOR3 orig, ox, oy, oz;
   D3DXVec3TransformCoord( &orig, &D3DXVECTOR3( 0, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &ox, &D3DXVECTOR3( SIZE, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &oy, &D3DXVECTOR3( 0, SIZE, 0 ), &mtx );
   D3DXVec3TransformCoord( &oz, &D3DXVECTOR3( 0, 0, SIZE ), &mtx );

   renderer.drawLine( orig, ox, OX_COLOR ); // OX
   renderer.drawLine( orig, oy, OY_COLOR ); // OY
   renderer.drawLine( orig, oz, OZ_COLOR ); // OZ
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::drawRotationGizmo( IDebugDraw& renderer ) const
{
   const D3DXMATRIX& mtx = m_node.getGlobalMtx();

   D3DXVECTOR3 orig, ox, oy, oz;
   D3DXVec3TransformCoord( &orig, &D3DXVECTOR3( 0, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &ox, &D3DXVECTOR3( SIZE, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &oy, &D3DXVECTOR3( 0, SIZE, 0 ), &mtx );
   D3DXVec3TransformCoord( &oz, &D3DXVECTOR3( 0, 0, SIZE ), &mtx );

   // OX
   renderer.drawLine( orig, ox, OX_COLOR );
   renderer.drawArc( ox, oz, OX_COLOR );

   // OY
   renderer.drawLine( orig, oy, OY_COLOR );
   renderer.drawArc( oy, oz, OX_COLOR );


   // OZ
   renderer.drawLine( orig, oz, OZ_COLOR );
   renderer.drawArc( oz, ox, OX_COLOR );
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::drawScalingGizmo( IDebugDraw& renderer ) const
{
   const D3DXMATRIX& mtx = m_node.getGlobalMtx();

   D3DXVECTOR3 orig, ox, oy, oz;
   D3DXVec3TransformCoord( &orig, &D3DXVECTOR3( 0, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &ox, &D3DXVECTOR3( SIZE, 0, 0 ), &mtx );
   D3DXVec3TransformCoord( &oy, &D3DXVECTOR3( 0, SIZE, 0 ), &mtx );
   D3DXVec3TransformCoord( &oz, &D3DXVECTOR3( 0, 0, SIZE ), &mtx );

   const D3DXVECTOR3 boxSize( 0.2f, 0.2f, 0.2f );

   // OX
   D3DXMATRIX boxMtx = mtx;
   boxMtx._41 = ox.x; boxMtx._42 = ox.y; boxMtx._43 = ox.z;
   renderer.drawLine( orig, ox, OX_COLOR );
   renderer.drawBox( boxMtx, boxSize, OX_COLOR );

   // OY
   boxMtx._41 = oy.x; boxMtx._42 = oy.y; boxMtx._43 = oy.z;
   renderer.drawLine( orig, oy, OY_COLOR );
   renderer.drawBox( boxMtx, boxSize, OY_COLOR );

   // OZ
   boxMtx._41 = oz.x; boxMtx._42 = oz.y; boxMtx._43 = oz.z;
   renderer.drawLine( orig, oz, OZ_COLOR );
   renderer.drawBox( boxMtx, boxSize, OZ_COLOR );
}

///////////////////////////////////////////////////////////////////////////////