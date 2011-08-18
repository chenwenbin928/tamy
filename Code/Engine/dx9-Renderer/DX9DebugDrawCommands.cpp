#include "core-Renderer\DebugDrawCommands.h"
#include "dx9-Renderer\DX9Renderer.h"
#include "core-Renderer\Camera.h"
#include "core\Color.h"
#include <stdexcept>

#define _USE_MATH_DEFINES
#include <math.h>


///////////////////////////////////////////////////////////////////////////////

void RCDrawLine::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
   dxRenderer.addDebugLine( m_start, m_end, m_color );
}

///////////////////////////////////////////////////////////////////////////////

void RCDrawArc::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void RCDrawArrow::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void RCDrawBox::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );

   static D3DXVECTOR3 vertices[8];
   static D3DXVECTOR3 transformedVertices[8];

   // define the box vertices
   // bottom face
   vertices[0].x = -m_size.x; vertices[0].y = -m_size.y; vertices[0].z = -m_size.z; 
   vertices[1].x =  m_size.x; vertices[1].y = -m_size.y; vertices[1].z = -m_size.z; 
   vertices[2].x =  m_size.x; vertices[2].y = -m_size.y; vertices[2].z =  m_size.z; 
   vertices[3].x = -m_size.x; vertices[3].y = -m_size.y; vertices[3].z =  m_size.z;

   // top face
   vertices[4].x = -m_size.x; vertices[4].y =  m_size.y; vertices[4].z = -m_size.z; 
   vertices[5].x =  m_size.x; vertices[5].y =  m_size.y; vertices[5].z = -m_size.z; 
   vertices[6].x =  m_size.x; vertices[6].y =  m_size.y; vertices[6].z =  m_size.z; 
   vertices[7].x = -m_size.x; vertices[7].y =  m_size.y; vertices[7].z =  m_size.z;

   // transform the vertices
   for ( char i = 0; i < 8; ++i )
   {
      D3DXVec3TransformCoord( &transformedVertices[i], &vertices[i], &m_transform );
   }

   // add the lines:
   // bottom face
   dxRenderer.addDebugLine( transformedVertices[0], transformedVertices[1], m_color );
   dxRenderer.addDebugLine( transformedVertices[1], transformedVertices[2], m_color );
   dxRenderer.addDebugLine( transformedVertices[2], transformedVertices[3], m_color );
   dxRenderer.addDebugLine( transformedVertices[3], transformedVertices[0], m_color );

   // top face
   dxRenderer.addDebugLine( transformedVertices[4], transformedVertices[5], m_color );
   dxRenderer.addDebugLine( transformedVertices[5], transformedVertices[6], m_color );
   dxRenderer.addDebugLine( transformedVertices[6], transformedVertices[7], m_color );
   dxRenderer.addDebugLine( transformedVertices[7], transformedVertices[4], m_color );

   // side lines
   dxRenderer.addDebugLine( transformedVertices[0], transformedVertices[4], m_color );
   dxRenderer.addDebugLine( transformedVertices[1], transformedVertices[5], m_color );
   dxRenderer.addDebugLine( transformedVertices[2], transformedVertices[6], m_color );
   dxRenderer.addDebugLine( transformedVertices[3], transformedVertices[7], m_color );
}

///////////////////////////////////////////////////////////////////////////////

void RCDrawSphere::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void RCDrawCylinder::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
}

///////////////////////////////////////////////////////////////////////////////

void RCRenderDebugScene::execute( Renderer& renderer )
{
   DX9Renderer& dxRenderer = dynamic_cast< DX9Renderer& >( renderer );
   dxRenderer.flushDebugScene();
}

///////////////////////////////////////////////////////////////////////////////
