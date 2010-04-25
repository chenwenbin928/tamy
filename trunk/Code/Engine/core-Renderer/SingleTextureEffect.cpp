#include "core-Renderer\SingleTextureEffect.h"
#include "core-Renderer\UniqueObjectAttribute.h"
#include "core-Renderer\Renderable.h"
#include "core-Renderer\Material.h"
#include "core-Renderer\Texture.h"
#include "core-Renderer\Camera.h"
#include "core-Renderer\CameraComponent.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT(SingleTextureEffect, ShaderEffect)
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

SingleTextureEffect::SingleTextureEffect()
: m_renderable(NULL)
, m_camera(NULL)
, m_texture(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::setMaterial(const Material& material)
{
   m_material = material;
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::setTexture(Texture& texture)
{
   m_texture = &texture;

   // TODO: przy zmianie tekstury trzeba zupdate'owac atrybuty i widoki
/*
   if (m_texture != NULL)
   {
      addAttribute(new UniqueObjectAttribute(*m_texture));
   }*/
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::onBeginRendering()
{
   // TODO: za duzo obliczen view matrix itd.

   if ((m_renderable != NULL) && (m_camera != NULL))
   {
      D3DXMATRIX worldViewMtx = m_renderable->getGlobalMtx() * m_camera->getViewMtx();
      shader().setMtx("g_mWorldView", worldViewMtx);
      shader().setMtx("g_mProjection", m_camera->getProjectionMtx());
   }

   shader().setVec4("g_MaterialAmbientColor", reinterpret_cast<const D3DXVECTOR4&> (m_material.getAmbientColor()));
   shader().setVec4("g_MaterialDiffuseColor", reinterpret_cast<const D3DXVECTOR4&> (m_material.getDiffuseColor()));

   shader().setBool("g_UseTexture", (m_texture != NULL));
   if (m_texture != NULL)
   {
      shader().setTexture("g_MeshTexture", *m_texture);
   }

   shader().setTechnique("singleTextureRenderer");
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::onAttached(Entity& parent)
{
   m_renderable = dynamic_cast<Renderable*> (&parent);
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::onDetached(Entity& parent)
{
   m_renderable = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::onAttached(Model& hostModel)
{
   CameraComponent* camComp = hostModel.getComponent<CameraComponent> ();
   if (camComp != NULL)
   {
      m_camera = &(camComp->getCamera());
   }
}

///////////////////////////////////////////////////////////////////////////////

void SingleTextureEffect::onDetached(Model& hostModel)
{
   m_camera = NULL;
}

///////////////////////////////////////////////////////////////////////////////