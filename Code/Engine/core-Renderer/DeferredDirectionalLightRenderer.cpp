#include "core-Renderer\DeferredDirectionalLightRenderer.h"
#include "core\MatrixUtils.h"
#include "core\ResourcesManager.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\PixelShader.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Defines.h"
#include "core-Renderer\FullscreenQuad.h"
#include "core-Renderer\BasicRenderCommands.h"
#include "core-Renderer\RenderTarget.h"
#include "core-Renderer\DepthBuffer.h"
#include "core-Renderer\RenderingView.h"
#include "core-Renderer\Geometry.h"
#include "core-Renderer\VertexShader.h"
#include "core-Renderer\VertexShaderConfigurator.h"
#include "core-Renderer\CascadedShadowsUtils.h"
#include "core-Renderer\DirectionalLight.h"
#include "core-Renderer\DeferredLightingRenderData.h"
#include "core-Renderer\ShaderUtils.h"


///////////////////////////////////////////////////////////////////////////////

#define MAX_CASCADES 8

///////////////////////////////////////////////////////////////////////////////

DeferredDirectionalLightRenderer::DeferredDirectionalLightRenderer( uint cascadesCount, uint cascadesPerRow )
   : m_cascadeCalculationConfig( cascadesCount, cascadesPerRow, 3.0f )
{
   m_calculatedCascadeStages = new CascadeStage[cascadesCount];

   m_visibleGeometry.resize( 1024, NULL );

   m_cascadeCalculationConfig.m_cascadeIntervals[0] = 0.0f;
   m_cascadeCalculationConfig.m_cascadeIntervals[1] = 10.0f;
   m_cascadeCalculationConfig.m_cascadeIntervals[2] = 30.0f;
   m_cascadeCalculationConfig.m_cascadeIntervals[3] = 60.0f;
   m_cascadeCalculationConfig.m_cascadeIntervals[4] = 120.0f;

   // load lighting shaders
   ResourcesManager& resMgr = ResourcesManager::getInstance();

   FilePath psPath( LIGHTING_SHADERS_DIR "Deferred/Directional/directionalLight.tpsh" );
   m_lightingShader = resMgr.create< PixelShader >( psPath, true );

   // load shadow shaders
   FilePath shadowDepthMapShaderPath( LIGHTING_SHADERS_DIR "Deferred/Directional/shadowDepthMapShader.tpsh" );
   m_shadowDepthMapShader = resMgr.create< PixelShader >( shadowDepthMapShaderPath, true );

   FilePath shadowProjectionPSPath( LIGHTING_SHADERS_DIR "Deferred/Directional/shadowProjectionShader.tpsh" );
   m_shadowProjectionPS = resMgr.create< PixelShader >( shadowProjectionPSPath, true );
}

///////////////////////////////////////////////////////////////////////////////

DeferredDirectionalLightRenderer::~DeferredDirectionalLightRenderer()
{
   delete [] m_calculatedCascadeStages;
   m_calculatedCascadeStages = NULL;

   // resources manager will take care of these objects in due time
   m_lightingShader = NULL;
   m_shadowDepthMapShader = NULL;
   m_shadowProjectionPS = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::render( Renderer& renderer, const DirectionalLight* light, const DeferredLightingRenderData& data )
{
   if ( !m_lightingShader )
   {
      return;
   }

   // first - render a shadow map, if this light is set to cast shadows
   bool drawShadows = light->castsShadows();
   if ( drawShadows )
   {
      renderShadowMap( renderer, light, data );
   }

   // and now render the light contribution
   renderLighting( renderer, light, data, drawShadows );
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::renderLighting( Renderer& renderer, const DirectionalLight* light, const DeferredLightingRenderData& data, bool drawShadows )
{
   if ( !data.m_sceneColorTex )
   {
      return;
   }

   // activate the final render target
   new ( renderer() ) RCActivateRenderTarget( data.m_finalLightColorTarget );

   // set and configure the pixel shader
   RCBindPixelShader* psComm = new ( renderer() ) RCBindPixelShader( *m_lightingShader, renderer );
   {
      const Matrix& globalMtx = light->getGlobalMtx();

      Vector halfPixel;
      ShaderUtils::calculateHalfPixel( renderer, data.m_depthTex, halfPixel );

      Camera& activeCamera = renderer.getActiveCamera();
      Vector lightDirVS;
      activeCamera.getViewMtx().transformNorm( globalMtx.forwardVec(), lightDirVS );
      psComm->setVec4( "g_halfPixel", halfPixel );
      psComm->setVec4( "g_lightDirVS", lightDirVS );
      psComm->setVec4( "g_lightColor", (const Vector&)light->m_color );
      psComm->setFloat( "g_strength", light->m_strength );

      psComm->setTexture( "g_Depth", data.m_depthTex );
      psComm->setTexture( "g_Normals", data.m_normalsTex );
      psComm->setTexture( "g_Speculars", data.m_specularTex );
      psComm->setTexture( "g_SceneColor", data.m_sceneColorTex );
      psComm->setInt( "g_materialsTexSize", data.m_materialsDescriptorsTex->getWidth() );
      psComm->setTexture( "g_MaterialIndices", data.m_materialIndicesTex );
      psComm->setTexture( "g_MaterialsDescr", data.m_materialsDescriptorsTex );

      psComm->setBool( "g_drawShadows", drawShadows );
      if ( data.m_screenSpaceShadowMap )
      {
         psComm->setTexture( "g_ShadowMap", data.m_screenSpaceShadowMap );
      }
   }

   // draw the geometry
   uint quadWidth = renderer.getViewportWidth();
   uint quadHeight = renderer.getViewportHeight();
   new ( renderer() ) RCFullscreenQuad( quadWidth, quadHeight );

   // cleanup
   new ( renderer() ) RCUnbindPixelShader( *m_lightingShader, renderer );
   new ( renderer() ) RCDeactivateRenderTarget();
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::renderShadowMap( Renderer& renderer, const DirectionalLight* light, const DeferredLightingRenderData& data )
{
   if ( !m_shadowDepthMapShader || !m_shadowProjectionPS || !data.m_shadowDepthTexture )
   {
      return;
   }

   Camera& activeCamera = renderer.getActiveCamera();

   m_cascadeCalculationConfig.m_lightRotationMtx = light->getGlobalMtx();
   m_cascadeCalculationConfig.m_lightRotationMtx.setPosition<3>( Quad_0 );

   m_cascadeCalculationConfig.m_cascadeDimensions = (float)( data.m_shadowDepthTexture->getWidth() ) / (float)m_cascadeCalculationConfig.m_numCascadesPerTextureRow;
   data.m_renderingView->getSceneBounds( m_cascadeCalculationConfig.m_sceneBounds );
   m_cascadeCalculationConfig.m_activeCamera = &activeCamera;

   Vector lightPos;
   CascadedShadowsUtils::calculateCascadesBounds( m_cascadeCalculationConfig, m_calculatedCascadeStages );

   // set the light camera
   Camera lightCamera( "dirLightCamera", renderer, Camera::PT_ORTHO );

   // render cascades
   renderCascades( renderer, activeCamera, lightCamera, data );

   // combine the cascades
   combineCascades( renderer, data, activeCamera, lightCamera, m_cascadeCalculationConfig.m_cascadeDimensions );
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::renderCascades( Renderer& renderer, Camera& activeCamera, Camera& lightCamera, const DeferredLightingRenderData& data )
{
   if ( !data.m_shadowDepthTexture || !data.m_shadowDepthSurface )
   {
      return;
   }

   // bind the shader and set the render target
   new ( renderer() ) RCActivateRenderTarget( data.m_shadowDepthTexture );
   new ( renderer() ) RCActivateDepthBuffer( data.m_shadowDepthSurface );
   new ( renderer() ) RCBindPixelShader( *m_shadowDepthMapShader, renderer );
   new ( renderer() ) RCClearDepthBuffer();

   // render cascades
   AABoundingBox expandedCascadeBounds;
   for ( int cascadeIdx = 0; cascadeIdx < m_cascadeCalculationConfig.m_numCascades; ++cascadeIdx )
   {
      const AABoundingBox& cascadeBounds = m_calculatedCascadeStages[cascadeIdx].m_lightFrustumBounds;
      lightCamera.setNearPlaneDimensions( cascadeBounds.max[0] - cascadeBounds.min[0], cascadeBounds.max[1] - cascadeBounds.min[1] );
      lightCamera.setClippingPlanes( cascadeBounds.min[2], cascadeBounds.max[2] );
      lightCamera.setLocalMtx( m_calculatedCascadeStages[cascadeIdx].m_lightMtx );

      new ( renderer() ) RCSetViewport( m_calculatedCascadeStages[cascadeIdx].m_viewport );

      // render the scene with the new camera
      {
         VSSetter vsSetter( lightCamera );

         m_visibleGeometry.clear();
         data.m_renderingView->collectRenderables( m_calculatedCascadeStages[cascadeIdx].m_objectsQueryBounds, m_visibleGeometry );

         uint sceneElemsCount = m_visibleGeometry.size();
         for ( uint i = 0; i < sceneElemsCount; ++i )
         {
            m_visibleGeometry[i]->render( renderer, &vsSetter );
         }
      }
   }

   // clean up
   new ( renderer() ) RCResetViewport();
   new ( renderer() ) RCUnbindPixelShader( *m_shadowDepthMapShader, renderer );
   new ( renderer() ) RCDeactivateDepthBuffer( data.m_shadowDepthSurface );
   new ( renderer() ) RCDeactivateRenderTarget();
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::combineCascades( Renderer& renderer, const DeferredLightingRenderData& data, Camera& activeCamera, Camera& lightCamera, float cascadeDimensions )
{
   if ( !data.m_screenSpaceShadowMap )
   {
      return;
   }

   uint numCascades = m_cascadeCalculationConfig.m_numCascades;

   float depthRanges[MAX_CASCADES + 1];
   Vector viewportOffsets[MAX_CASCADES];
   Matrix clipToLightSpaceMtx[MAX_CASCADES];

   float shadowMapDimension = (float)data.m_shadowDepthTexture->getWidth();

   Matrix worldToClipSpace;
   worldToClipSpace.setMul( activeCamera.getViewMtx(), activeCamera.getProjectionMtx() );

   Matrix invClipSpace;
   invClipSpace.setInverse( worldToClipSpace );

   for( uint i = 0; i < numCascades; ++i )
   {
      CascadeStage& stage = m_calculatedCascadeStages[i];
      depthRanges[i] = stage.m_cameraFarZ;
      viewportOffsets[i].set( (float)stage.m_viewport.m_offsetX / shadowMapDimension, (float)stage.m_viewport.m_offsetY / shadowMapDimension, 0 );

      // calculate the transformation matrix
      {
         const AABoundingBox& cascadeBounds = stage.m_lightFrustumBounds;
         lightCamera.setNearPlaneDimensions( cascadeBounds.max[0] - cascadeBounds.min[0], cascadeBounds.max[1] - cascadeBounds.min[1] );
         lightCamera.setClippingPlanes( cascadeBounds.min[2], cascadeBounds.max[2] );
         lightCamera.setLocalMtx( m_calculatedCascadeStages[i].m_lightMtx );

         Matrix lightViewProj;
         lightViewProj.setMul( lightCamera.getViewMtx(), lightCamera.getProjectionMtx() );

         clipToLightSpaceMtx[i].setMul( invClipSpace, lightViewProj );     
      }
   }
   depthRanges[m_cascadeCalculationConfig.m_numCascades] = FLT_MAX;

   //  render the shadow projection map
   {
      new ( renderer() ) RCActivateRenderTarget( data.m_screenSpaceShadowMap );
      RCBindPixelShader* psComm = new ( renderer() ) RCBindPixelShader( *m_shadowProjectionPS, renderer );
      {
         // set the shadow map
         float texelDimension = 1.0f / cascadeDimensions;
         float cascadeScale = cascadeDimensions / shadowMapDimension;
         psComm->setFloat( "g_texelDimension", texelDimension );
         psComm->setTexture( "g_shadowDepthMap", data.m_shadowDepthTexture );
         psComm->setFloat( "g_cascadeScale", cascadeScale );
         psComm->setFloat( "g_cascadeDepthRanges", depthRanges, numCascades + 1 );
         psComm->setVec4( "g_cascadeOffsets", viewportOffsets, numCascades  );
         psComm->setMtx( "g_clipToLightSpaceMtx", clipToLightSpaceMtx, numCascades );
      }

      // render visible scene elements
      uint sceneElemsCount = data.m_geometryToRender->size();
      for ( uint i = 0; i < sceneElemsCount; ++i )
      {
         (*data.m_geometryToRender)[i]->render( renderer );
      }

      new ( renderer() ) RCUnbindPixelShader( *m_shadowProjectionPS, renderer );
      new ( renderer() ) RCDeactivateRenderTarget();
   }

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DeferredDirectionalLightRenderer::VSSetter::VSSetter( Camera& lightCamera )
{
   m_lightViewProjMtx.setMul( lightCamera.getViewMtx(), lightCamera.getProjectionMtx() );
}

///////////////////////////////////////////////////////////////////////////////

void DeferredDirectionalLightRenderer::VSSetter::configure( const Geometry& geometry, RCBindVertexShader* command )
{
   const Matrix& geometryWorldMtx = geometry.getGlobalMtx();

   Matrix lightViewProjMtx;
   lightViewProjMtx.setMul( geometryWorldMtx, m_lightViewProjMtx );

   command->setMtx( "g_matLightViewProj", lightViewProjMtx );
}

///////////////////////////////////////////////////////////////////////////////
