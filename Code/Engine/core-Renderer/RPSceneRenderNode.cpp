#include "core-Renderer/RPSceneRenderNode.h"
#include "core-Renderer/RenderingPipelineMechanism.h"
#include "core-Renderer/Renderer.h"
#include "core-Renderer/RenderingPipelineSockets.h"
#include "core-Renderer/RenderTarget.h"
#include "core-Renderer/RenderingView.h"
#include "core-Renderer/RPSBTextured.h"
#include "core/MemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPSceneRenderNode );
   PARENT( RenderingPipelineNode );
   PROPERTY_EDIT( "Rendered scene id", RPMSceneId, m_renderedSceneId );
   PROPERTY_EDIT( "Scene contents builder", RPSceneBuilder*, m_builder );
   PROPERTY_EDIT( "Render target id", std::string, m_renderTargetId );
   PROPERTY_EDIT( "Clear depth buffer", bool, m_clearDepthBuffer );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::RPSceneRenderNode()
   : m_renderedSceneId( RPS_Main )
   , m_builder( NULL )
   , m_clearDepthBuffer( true )
{
   defineInput( new RPVoidInput( "Input" ) );
   defineOutput( new RPTextureOutput( "Output" ) );
}
///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::~RPSceneRenderNode()
{
   delete m_builder; m_builder = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onPropertyChanged( ReflectionProperty& property )
{
   __super::onPropertyChanged( property );

   if ( property.getName() == "m_builder" && m_builder )
   {
      m_builder->defineSockets( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onCreateLayout( RenderingPipelineMechanism& host ) const
{
   RuntimeDataBuffer& data = host.data();

   // register runtime members
   data.registerVar( m_renderTarget );
   data.registerVar( m_treeMemPool );

   // create the render target
   RenderTarget* trg = NULL;
   {
      trg = host.getRenderTarget( m_renderTargetId );
      data[ m_renderTarget ] = trg;
   }

   // find the existing outputs and set the data on them
   {
      RPTextureOutput* output = DynamicCast< RPTextureOutput >( findOutput( "Output" ) );
      output->setValue( data, trg );
   }

   // create a memory pool for the render tree
   {
      MemoryPool* treeMemPool = new MemoryPool( 1024 * 1024 );
      data[ m_treeMemPool ] = treeMemPool;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onDestroyLayout( RenderingPipelineMechanism& host ) const
{
   RuntimeDataBuffer& data = host.data();

   MemoryPool* treeMemPool = data[ m_treeMemPool ];
   delete treeMemPool;
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onUpdate( RenderingPipelineMechanism& host ) const
{
   if ( !host.isSceneActive( m_renderedSceneId ) || !m_builder )
   {
      // no scene - no rendering
      return;
   }

   RuntimeDataBuffer& data = host.data();
   RenderTarget* trg = data[ m_renderTarget ];
   Renderer& renderer = host.getRenderer();
   MemoryPool* treeMemPool = data[ m_treeMemPool ];

   // activate the render target we want to render to
   new ( renderer() ) RCActivateRenderTarget( trg );

   // collect the renderables
   const Array< Geometry* >& visibleElems = host.getSceneElements( m_renderedSceneId );

   // build a tree sorting the nodes by the attributes
   treeMemPool->reset();
   StateTreeNode* root = m_builder->buildRenderTree( *treeMemPool, visibleElems, data );

   if ( root )
   {
      if ( m_clearDepthBuffer )
      {
         new ( renderer() ) RCClearDepthBuffer();
      }

      // render the tree contents
      root->render( renderer );

      // get rid of the tree
      MEMPOOL_DELETE( root );
   }
}

///////////////////////////////////////////////////////////////////////////////
