#include "core-Renderer/RPSceneRenderNode.h"
#include "core-Renderer/RenderingPipelineMechanism.h"
#include "core-Renderer/Renderer.h"
#include "core-Renderer/VoidSockets.h"
#include "core-Renderer/TextureSockets.h"
#include "core-Renderer/RenderTarget.h"
#include "core-Renderer/RenderingView.h"
#include "core-Renderer/SpatialRepresentation.h"
#include "core-Renderer/RPSBTextured.h"
#include "core/MemoryPool.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( RPSceneRenderNode, RenderingPipelineNode )
   PROPERTY_EDIT( "Rendered scene id", RPMSceneId, m_renderedSceneId );
   PROPERTY_EDIT( "Scene contents builder", RPSceneBuilder*, m_builder );
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::RPSceneRenderNode()
   : m_renderedSceneId( RPS_Main )
   , m_builder( NULL )
   , m_treeMemPool( new MemoryPool( 1024 * 1024 ) )
{
   defineInput( new RPVoidInput( "Input" ) );
   defineOutput( new RPRenderTargetOutput( "Output" ) );
}
///////////////////////////////////////////////////////////////////////////////

RPSceneRenderNode::~RPSceneRenderNode()
{
   delete m_builder; m_builder = NULL;
   delete m_treeMemPool; m_treeMemPool = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onPropertyChanged( Property& property )
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
   host.data().registerVar( m_renderTarget );
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onInitialize( RenderingPipelineMechanism& host ) const
{
   host.data()[ m_renderTarget ] = getOutput< RPRenderTargetOutput >( "Output" ).getRenderTarget( host.data() );
}

///////////////////////////////////////////////////////////////////////////////

void RPSceneRenderNode::onDeinitialize( RenderingPipelineMechanism& host ) const
{
   host.data()[ m_renderTarget ] = NULL;
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
  
   RenderingView& sceneRenderer = host.getSceneRenderer( m_renderedSceneId );

   // activate the render target we want to render to
   new ( renderer() ) RCActivateRenderTarget( trg );

   // collect the renderables
   Array< SpatialRepresentation* > visibleElems;
   sceneRenderer.collectRenderables( visibleElems );

   // build a tree sorting the nodes by the attributes
   m_treeMemPool->reset();
   StateTreeNode* root = m_builder->buildRenderTree( *m_treeMemPool, visibleElems );

   if ( root )
   {
      // render the tree contents
      root->render( renderer, data );

      // get rid of the tree
      MEMPOOL_DELETE( root );
   }
}

///////////////////////////////////////////////////////////////////////////////
