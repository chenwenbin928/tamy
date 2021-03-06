/// @file   core-Renderer/PixelShaderNodeOperator.h
/// @brief  Operator for nodes that should be parametrized according to the pixel shader they are using.
#ifndef _PIXEL_SHADER_NODE_OPERATOR_H
#define _PIXEL_SHADER_NODE_OPERATOR_H

#include <vector>
#include "core-Renderer/PixelShaderConstant.h"

///////////////////////////////////////////////////////////////////////////////

class PixelShader;
class Renderer;
class RuntimeDataBuffer;
class RCBindPixelShader;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class provides a pixel shader with a set of node connections
 * necessary for it to have its global constants set.
 */
template< typename TNode >
class PixelShaderNodeOperator
{
   DECLARE_ALLOCATOR( PixelShaderNodeOperator, AM_DEFAULT );

private:
   struct ConstantDef
   {
      DECLARE_ALLOCATOR( ConstantDef, AM_DEFAULT );

      PixelShaderConstant< TNode >*    m_constant;
      GBNodeInput< TNode >*            m_input;

      ConstantDef( PixelShaderConstant< TNode >* constant, GBNodeInput< TNode >* input );
      ~ConstantDef();
   };

private:
   TNode&                              m_hostNode;
   PixelShader*                        m_shader;
   std::vector< ConstantDef* >         m_constants;

public:
   /**
    * Constructor.
    *
    * @param hostNode
    */
   PixelShaderNodeOperator( TNode& hostNode );
   ~PixelShaderNodeOperator();

   /**
    * Sets a new shader for the node to use.
    *
    * @param shader
    */
   void setShader( PixelShader& shader );

   /**
    * Removes the previously set shader instance.
    */
   void resetShader();

   /**
    * Filters the sockets that DON'T belong to the current set of pixel shader constants.
    *
    * @param inOutSocketName
    */
   void filterSockets( std::vector< std::string >& inOutSocketNames ) const;

   /**
    * Propagates the data from the input to the assigned shader constant
    *
    * @param renderer
    * @param data       runtime data buffer the inputs are using
    *
    * @return           rendering command that binds the pixel shader
    */
   RCBindPixelShader& bindShader( Renderer& renderer, RuntimeDataBuffer& data );

private:
   /**
    * This helper methods builds the actual node sockets for the specified shader constants.
    *
    * @param constants
    * @param outConstantsSockets
    */
   void buildConstantsSockets( const std::vector< ShaderConstantDesc >& constants, std::vector< PixelShaderConstant< TNode >* >& outConstantsSockets ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-Renderer/PixelShaderNodeOperator.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _PIXEL_SHADER_NODE_OPERATOR_H
