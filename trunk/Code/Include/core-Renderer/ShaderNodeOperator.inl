#ifndef _SHADER_NODE_OPERATOR_H
#error "This file can only be included from ShaderNodeOperation.h"
#else


#include "core-Renderer/ShaderNodeOperator.h"
#include "core-Renderer/PixelShaderConstant.h"
#include "core-Renderer/PixelShader.h"
#include "core-Renderer/RenderingPipelineNode.h"
#include "core-Renderer/RenderingPipelineSockets.h"
#include "core-Renderer/Renderer.h"


///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
ShaderNodeOperator< TNode >::ShaderNodeOperator( TNode& hostNode )
   : m_hostNode( hostNode )
   , m_shader( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
ShaderNodeOperator< TNode >::~ShaderNodeOperator()
{
   resetShader();
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void ShaderNodeOperator< TNode >::setShader( PixelShader& shader )
{
   resetShader();

   m_shader = &shader;

   const std::vector< PixelShaderConstant* >& constants = m_shader->getConstants();
   unsigned int count = constants.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_constants.push_back( new ConstantDef( *constants[i] ) );
      m_constants.back()->setHostNode( &m_hostNode );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void ShaderNodeOperator< TNode >::resetShader()
{
   unsigned int count = m_constants.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_constants[i]->setHostNode( NULL );
      delete m_constants[i];
   }
   m_constants.clear();

   m_shader = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
 RCBindPixelShader& ShaderNodeOperator< TNode >::bindShader( Renderer& renderer, RuntimeDataBuffer& data )
{
   RCBindPixelShader* comm = new ( renderer() ) RCBindPixelShader( *m_shader );

   // set the shader constants
   unsigned int count = m_constants.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      PixelShaderConstant& constant = m_constants[i]->m_constant;
      RPNodeInput* input = m_constants[i]->m_input;
      constant.setValue( *comm, *input, data );
   }

   return *comm;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
ShaderNodeOperator< TNode >::ConstantDef::ConstantDef( PixelShaderConstant& constant ) 
   : m_constant( constant )
   , m_input( NULL )
   , m_hostNode( NULL )
{}

///////////////////////////////////////////////////////////////////////////////

template< typename TNode >
void ShaderNodeOperator< TNode >::ConstantDef::setHostNode( TNode* hostNode )
{
   const std::string& inputName = m_constant.getName();

   if ( m_hostNode )
   {
      // remove it from the old node
      m_hostNode->removeInput( inputName );
   }

   m_hostNode = hostNode;

   if ( m_hostNode )
   {
      // add it to the new node
      RPNodeInput* input = m_hostNode->findInput( inputName );
      if ( input && !m_constant.doesTypeMatch( *input ) )
      {
         m_hostNode->removeInput( inputName );
         input = NULL;
      }

      if ( !input )
      {
         input = m_constant.createRPNInput();
         m_hostNode->defineInput( input );
      }

      m_input = input;
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _SHADER_NODE_OPERATOR_H