#pragma once

#include "core-Renderer\GraphicalEntitiesFactory.h"
#include "core-Renderer\ManagedTexture.h"
#include <string>
#include "RendererStub.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\MaterialStage.h"
#include <vector>
#include <algorithm>
#include <cassert>


///////////////////////////////////////////////////////////////////////////////

template<>
class TTextureImplDestructor<int>
{
public:
   void destroy(int* tex) {}
};

///////////////////////////////////////////////////////////////////////////////

class MaterialsFactoryMock : public GraphicalEntitiesFactory
{
private:
   static RendererStub s_renderer;

public:
   MaterialsFactoryMock() : GraphicalEntitiesFactory("", s_renderer) {}

   Light* createLight(const std::string& name)
   {
       return NULL;
   }

   ParticleSystem* createParticleSystem(const std::string& name, 
                                        bool isDynamic, 
                                        Material& material,
                                        unsigned int particlesCount)
   {
       return NULL;
   }

   SceneRenderingMechanism* createSceneRenderingMechanism(RenderingTargetsPolicy* policy)
   {
      return NULL;
   }

protected:
   GraphicalEntity* createGraphicalEntity(const std::string& name,
                                          const std::vector<LitVertex>& vertices,
                                          const std::list<Face<USHORT> >& faces,
                                          const std::vector<Material*>& materials)
    {
       return NULL;
   }

   SkinnedGraphicalEntity* createSkinnedGraphicalEntity(const std::string& name,
                                                        const std::vector<LitVertex>& vertices,
                                                        const std::list<Face<USHORT> >& faces,
                                                        const std::vector<BonesInfluenceDefinition>& bonesInfluencingAttribute,
                                                        const std::vector<SkinBoneDefinition>& skinBones,
                                                        const std::vector<Material*>& materials)
    {
       return NULL;
   }

   SkyBox* createSkyBox() 
   {
      return NULL;
   }

   Texture* loadTexture(const std::string& path, const std::string& fileName)
   {
      return new ManagedTexture(fileName, new TTextureImpl<int>());
   }

   Texture* createEmptyTexture()
   {
      return new ManagedTexture("", new TTextureImpl<int>());
   }

   RenderingTarget* createDefaultRenderingTarget() {return NULL;}

   TextureRenderingTarget* createTextureRenderingTarget(const std::string& name) {return NULL;}

   GraphicalEffect* createEffectImpl(const std::string& name,
                                     RenderingTargetsPolicy& policy,
                                     EffectDataSource* dataSource)
   {
      return NULL;
   }

   PostProcessEffectRenderable* createPostProcessEffectRenderable(void)
   {
      return NULL;
   }
};

///////////////////////////////////////////////////////////////////////////////
