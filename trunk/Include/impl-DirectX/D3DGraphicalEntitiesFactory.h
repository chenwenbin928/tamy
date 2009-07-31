#pragma once

#include "core-Renderer\GraphicalEntitiesFactory.h"
#include <d3d9.h>


///////////////////////////////////////////////////////////////////////////////

class D3DRenderer;
class D3DColorOperationImplementation;
class D3DAlphaOperationImplementation;
class D3DTransparencyEnabler;
class D3DStageTextureRenderer;

///////////////////////////////////////////////////////////////////////////////

class D3DGraphicalEntitiesFactory : public GraphicalEntitiesFactory
{
private:
   IDirect3DDevice9& m_d3Device;
   D3DRenderer& m_renderer;
   D3DColorOperationImplementation* m_colorOpImpl;
   D3DAlphaOperationImplementation* m_alphaOpImpl;
   D3DTransparencyEnabler *m_transparencyEnabler;
   D3DStageTextureRenderer* m_stageTextureRenderer;

public:
   D3DGraphicalEntitiesFactory(const std::string& texturesPath,
                               IDirect3DDevice9& d3Device,
                               D3DRenderer& renderer);
   ~D3DGraphicalEntitiesFactory();

   Light* createLight(const std::string& name);

   LightReflectingProperties* createLightReflectingProperties();

   ParticleSystem* createParticleSystem(const std::string& name, 
                                        bool isDynamic, 
                                        RenderingTechnique& renderingTechnique,
                                        unsigned int particlesCount);

   RenderingTarget* createDefaultRenderingTarget();

   TextureRenderingTarget* createTextureRenderingTarget(const std::string& name);

   SceneRenderingMechanism* createSceneRenderingMechanism(RenderingTargetsPolicy* policy);

protected:
   GraphicalEntity* createGraphicalEntity(const std::string& name,
                                          const std::vector<LitVertex>& vertices,
                                          const std::list<Face<USHORT> >& faces,
                                          const std::vector<RenderingTechnique*>& techniques);

   SkinnedGraphicalEntity* createSkinnedGraphicalEntity(const std::string& name,
                                                        const std::vector<LitVertex>& vertices,
                                                        const std::list<Face<USHORT> >& faces,
                                                        const std::vector<BonesInfluenceDefinition>& bonesInfluencingAttribute,
                                                        const std::vector<SkinBoneDefinition>& skinBones,
                                                        const std::vector<RenderingTechnique*>& techniques);

   SkyBox* createSkyBox();

   Texture* loadTexture(const std::string& path, const std::string& fileName);
   Texture* createEmptyTexture();

   MaterialStage* createMaterialStage(Texture& tex,
                                      MatOpCode colorOp, SourceCode colorArg1, SourceCode colorArg2,
                                      MatOpCode alphaOp, SourceCode alphaArg1, SourceCode alphaArg2,
                                      CoordsOpCode coordsOp);

   Material* createMaterialImpl(const std::string& name,
                                RenderingTargetsPolicy& policy,
                                LightReflectingProperties* lrp);

   GraphicalEffect* createEffectImpl(const std::string& name,
                                     RenderingTargetsPolicy& policy,
                                     bool isTransparent,
                                     EffectDataSource* dataSource);

   PostProcessEffectRenderable* createPostProcessEffectRenderable();
};

///////////////////////////////////////////////////////////////////////////////
