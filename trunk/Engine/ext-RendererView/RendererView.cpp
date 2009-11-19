#include "ext-RendererView\RendererView.h"
#include "ext-RendererView\GraphicalRepresentation.h"
#include "ext-RendererView\LightsScene.h"
#include "core-Renderer\CompositeRenderingMechanism.h"
#include "core-Renderer\GraphicalEntitiesFactory.h"
#include "core-Renderer\TargetsCleaningMechanism.h"
#include "core-Renderer\GeometryRenderingMechanism.h"
#include "core-Renderer\SettableRenderingTargetsPolicy.h"
#include "core-Renderer\SortingRenderablesStorage.h"
#include "core-Renderer\RenderingActionsComposite.h"
#include "core-Renderer\RenderMaterial.h"
#include "core-Renderer\RenderLights.h"
#include "core-Renderer\RenderingTarget.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\Light.h"
#include "core-Renderer\StaticGeometryRenderable.h"
#include "core-Renderer\Renderer.h"
#include "core-Renderer\AbstractGraphicalEntity.h"
#include "core-Scene\Entity.h"


// scene entities
#include "core-Scene\CameraEntity.h"
#include "core-Scene\DirectionalLightEntity.h"
#include "core-Scene\PointLightEntity.h"
#include "core-Scene\SpotLightEntity.h"
#include "core-Scene\SkyBoxEntity.h"
#include "core-Scene\StaticGeometryEntity.h"
#include "core-Scene\MaterialEntity.h"
#include "core-Scene\AnimatedCutsceneEntity.h"

// graphical representations
#include "ext-RendererView\Camera.h"
#include "ext-RendererView\DirectionalLight.h"
#include "ext-RendererView\PointLight.h"
#include "ext-RendererView\SpotLight.h"
#include "ext-RendererView\SkyBox.h"
#include "ext-RendererView\StaticGeometry.h"
#include "ext-RendererView\Material.h"
#include "ext-RendererView\AnimatedCutscene.h"


namespace RendererView
{

///////////////////////////////////////////////////////////////////////////////

RendererView::RendererView(GraphicalEntitiesFactory& factory,
                           Renderer& renderer)
: m_factory(factory)
, m_renderer(renderer)
, m_lights(new LightsScene())
, m_textures(new ResourceStorage<Texture>())
, m_materials(new ResourceStorage<::Material>())
, m_dynamicEntities(new ResourceStorage<AbstractGraphicalEntity>())
, m_staticGeometry(new StaticSceneManager(AABoundingBox(D3DXVECTOR3(-2000, -2000, -2000), D3DXVECTOR3(2000, 2000, 2000)), 64, 5))
, m_dynamicGeometry(new DynamicSceneManager(new LinearStorage<RenderableNode>()))
, m_camera(new ::Camera("camera", m_renderer))
{
   m_screenTarget = m_factory.createDefaultRenderingTarget();

   // assemble the rendering pipeline
   CompositeRenderingMechanism* mechanism = new CompositeRenderingMechanism();

   // targets cleaner
   SettableRenderingTargetsPolicy* tcPolicy = new SettableRenderingTargetsPolicy();
   tcPolicy->addTarget(0, *m_screenTarget);
   RenderingTargetsCleaner* cleaner = m_factory.createTargetsCleaner();
   TargetsCleaningMechanism* targetsCleaner = new TargetsCleaningMechanism(tcPolicy, cleaner);
   mechanism->add(targetsCleaner);

   // static objects renderer
   {
      SettableRenderingTargetsPolicy* statScenePolicy = new SettableRenderingTargetsPolicy();
      statScenePolicy->addTarget(0, *m_screenTarget);


      RendererImpl* rendererImpl = m_factory.createFixedRendererImpl();
      GeometryRenderingMechanism* statSceneRenderer = new GeometryRenderingMechanism(statScenePolicy, rendererImpl);

      statSceneRenderer->setActiveCamera(*m_camera);

      statSceneRenderer->setStorage(new SortingRenderablesStorage<StaticGeometryRenderable>(*m_staticGeometry));

      RenderingActionsComposite* renderingAction = new RenderingActionsComposite();
      renderingAction->addAction(new RenderMaterial());
      if (m_lights != NULL) {renderingAction->addAction(new RenderLights(*m_lights));}
      statSceneRenderer->setRenderingAction(renderingAction);

      mechanism->add(statSceneRenderer);
   }

   // dynamic objects renderer
   {
      SettableRenderingTargetsPolicy* statScenePolicy = new SettableRenderingTargetsPolicy();
      statScenePolicy->addTarget(0, *m_screenTarget);

      RendererImpl* rendererImpl = m_factory.createFixedRendererImpl();
      GeometryRenderingMechanism* statSceneRenderer = new GeometryRenderingMechanism(statScenePolicy, rendererImpl);

      statSceneRenderer->setActiveCamera(*m_camera);

      statSceneRenderer->setStorage(new SortingRenderablesStorage<RenderableNode>(*m_dynamicGeometry));

      RenderingActionsComposite* renderingAction = new RenderingActionsComposite();
      renderingAction->addAction(new RenderMaterial());
      if (m_lights != NULL) {renderingAction->addAction(new RenderLights(*m_lights));}
      statSceneRenderer->setRenderingAction(renderingAction);

      mechanism->add(statSceneRenderer);
   }

   m_renderer.setMechanism(mechanism);

   // register entities
   associate<CameraEntity,            Camera> ()
   .associate<DirectionalLightEntity, DirectionalLight> ()
   .associate<PointLightEntity,       PointLight> ()
   .associate<SpotLightEntity,        SpotLight> ()
   .associate<SkyBoxEntity,           SkyBox> ()
   .associate<StaticGeometryEntity,   StaticGeometry> ()
   .associate<MaterialEntity,         Material> ()
   .associate<AnimatedCutsceneEntity, AnimatedCutscene> ();
}

///////////////////////////////////////////////////////////////////////////////

RendererView::~RendererView()
{
   m_renderer.setMechanism(NULL);

   resetContents();

   delete m_lights; m_lights = NULL;
   delete m_textures; m_textures = NULL;
   delete m_materials; m_materials = NULL;
   delete m_dynamicEntities; m_dynamicEntities = NULL;
   delete m_staticGeometry; m_staticGeometry = NULL;
   delete m_dynamicGeometry; m_dynamicGeometry = NULL;
   delete m_screenTarget; m_screenTarget = NULL;
   delete m_camera; m_camera = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void RendererView::update(float timeElapsed)
{
   for (Representations::iterator it = m_representations.begin();
      it != m_representations.end(); ++it)
   {
      it->second->update(timeElapsed);
   }
}

///////////////////////////////////////////////////////////////////////////////

void RendererView::onEntityAdded(Entity& entity)
{
   Representations::iterator reprIt = m_representations.find(&entity);
   if (reprIt != m_representations.end())
   {
      // same entity instance can't have more than one graphical representation
      return;
   }

   GraphicalRepresentation* representation = create(entity);
   representation->initialize(*this);

   m_representations.insert(std::make_pair(&entity, representation));
}

///////////////////////////////////////////////////////////////////////////////

void RendererView::onEntityRemoved(Entity& entity)
{
   Representations::iterator reprIt = m_representations.find(&entity);
   if (reprIt != m_representations.end())
   {
      GraphicalRepresentation* representation = reprIt->second;
      m_representations.erase(reprIt);

      representation->deinitialize(*this);
      delete representation;
   }
}

///////////////////////////////////////////////////////////////////////////////

void RendererView::resetContents()
{
   for (Representations::iterator it = m_representations.begin();
        it != m_representations.end(); ++it)
   {
      delete it->second;
   }
   m_representations.clear();
}

///////////////////////////////////////////////////////////////////////////////

GraphicalEntitiesFactory& RendererView::factory() 
{
   return m_factory;
}

///////////////////////////////////////////////////////////////////////////////

LightsScene& RendererView::lights() 
{
   return *m_lights;
}

///////////////////////////////////////////////////////////////////////////////

ResourceStorage<::Material>& RendererView::materials()
{
   return *m_materials;
}

///////////////////////////////////////////////////////////////////////////////

ResourceStorage<Texture>& RendererView::textures()
{
   return *m_textures;
}

///////////////////////////////////////////////////////////////////////////////

ResourceStorage<AbstractGraphicalEntity>& RendererView::dynamicEntities()
{
   return *m_dynamicEntities;
}

///////////////////////////////////////////////////////////////////////////////

StaticSceneManager& RendererView::staticGeometry()
{
   return *m_staticGeometry;
}

///////////////////////////////////////////////////////////////////////////////

DynamicSceneManager& RendererView::dynamicGeometry()
{
   return *m_dynamicGeometry;
}

///////////////////////////////////////////////////////////////////////////////

::Camera& RendererView::camera()
{
   return *m_camera;
}

///////////////////////////////////////////////////////////////////////////////

} // ScenePlayer