#include "core-Renderer/GNVertexShader.h"
#include "core-Renderer/VertexShader.h"
#include "core-Renderer/GeometryEntity.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GNVertexShader, GeometryShaderNode )
   PROPERTY_EDIT( "Shader", VertexShader*, m_shader )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

GNVertexShader::GNVertexShader()
   : m_shader( NULL )
   , m_shaderNode( NULL )
{
   m_shaderNode = new VertexShaderNodeOperator< GeometryShaderNode >( *this );
}

///////////////////////////////////////////////////////////////////////////////

GNVertexShader::~GNVertexShader()
{
   delete m_shaderNode; m_shaderNode = NULL;
   m_shader = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void GNVertexShader::onObjectLoaded()
{
   __super::onObjectLoaded();

   if ( m_shader )
   {
      m_shaderNode->setShader( *m_shader );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GNVertexShader::onPrePropertyChanged( Property& property )
{
   __super::onPrePropertyChanged( property );

   if ( property.getName() == "m_shader" && m_shaderNode )
   {
      m_shaderNode->resetShader();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GNVertexShader::onPropertyChanged( Property& property )
{
   __super::onPropertyChanged( property );

   if ( property.getName() == "m_shader" && m_shader )
   {
      m_shaderNode->setShader( *m_shader );
   }
}

///////////////////////////////////////////////////////////////////////////////

void GNVertexShader::preRender( Renderer& renderer, const GeometryEntity& entity ) const
{
   if ( !m_shader || !m_shaderNode )
   {
      return;
   }

   // bind the shader
   m_shaderNode->bindShader( renderer, entity.data() );
}

///////////////////////////////////////////////////////////////////////////////

void GNVertexShader::postRender( Renderer& renderer, const GeometryEntity& entity ) const
{
   if ( m_shader )
   {
      new ( renderer() ) RCUnbindVertexShader( *m_shader );
   }
}

///////////////////////////////////////////////////////////////////////////////
