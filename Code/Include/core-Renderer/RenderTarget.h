/// @file   core-Renderer\RenderTarget.h
/// @brief  a special kind of texture which a renderer uses as an output
#pragma once

#include "core\Observer.h"
#include "core-Renderer\ShaderTexture.h"
#include "core-Renderer\RenderCommand.h"
#include "core-Renderer\ShaderParam.h"
#include "core-Renderer\RenderResource.h"
#include "core\UniqueObject.h"
#include "core\Color.h"
#include "core\Point.h"
#include "core\IDString.h"
#include <string>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Renderer;
enum RendererOps;
class RenderTargetSizePolicy;

///////////////////////////////////////////////////////////////////////////////

/**
 * A special kind of texture which a renderer uses as an output.
 */
class RenderTarget : public ShaderTexture, public UniqueObject< RenderTarget >, public RenderResource
{
   DECLARE_ALLOCATOR( RenderTarget, AM_DEFAULT );
   DECLARE_CLASS()

private:
   RenderTargetSizePolicy*       m_sizePolicy;
   TextureUsage                  m_usage;
   bool                          m_isReadable;
   Color                         m_bgColor;

   unsigned int                  m_width;
   unsigned int                  m_height;

public:
   /**
    * Constructor.
    *
    * @param sizePolicy    size policy
    * @param usage         render target usage
    * @param isReadable    tells if we can perform reading operations on the texture
    * @param bgColor       color the background of the target should be filled with
    */
   RenderTarget( RenderTargetSizePolicy* sizePolicy = NULL, TextureUsage usage = TU_COLOR, bool isReadable = false, const Color& bgColor = Color( 0.f, 0.f, 0.f, 0.f ) );
   ~RenderTarget();

   /**
    * Sets the new size of the render target.
    */
   void resize( unsigned int width, unsigned int height );

   /**
    * Returns the render target usage.
    */
   inline TextureUsage getUsage() const { return m_usage; }

   /**
    * Tells if the texture supports reading operation.
    */
   inline bool isReadable() const { return m_isReadable; }

   /**
    * Returns the color the target's background should be filled with
    * when it's being cleaned.
    */
   inline const Color& getBackgroundColor() const { return m_bgColor; }


   // -------------------------------------------------------------------------
   // ShaderTexture implementation
   // -------------------------------------------------------------------------
   inline unsigned int getWidth() const { return m_width; }
   inline unsigned int getHeight() const { return m_height; }
   inline ShaderParam< EffectShader >* createEffectShaderTextureSetter( MemoryPoolAllocator* allocator, const IDString& paramName ) { return new ( allocator ) ShaderParamRenderTarget< EffectShader >( paramName.getId(), *this ); }
   inline ShaderParam< PixelShader >* createPixelShaderTextureSetter( MemoryPoolAllocator* allocator, const IDString& paramName ) { return new ( allocator ) ShaderParamRenderTarget< PixelShader >( paramName.getId(), *this ); }
   inline ShaderParam< VertexShader >* createVertexShaderTextureSetter( MemoryPoolAllocator* allocator, const IDString& paramName ){ return new ( allocator ) ShaderParamRenderTarget< VertexShader >( paramName.getId(), *this ); }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Size policy scales a render target.
 */
class RenderTargetSizePolicy
{
public:
   virtual ~RenderTargetSizePolicy() {}

   /**
    * Initializes the policy.
    *
    * @param target        host render target
    */
   virtual void initialize( RenderTarget& target ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Dynamically adjusts render target's size to the size of an active viewport
 */
class RTSPDynamic : public RenderTargetSizePolicy, public Observer< Renderer, RendererOps >
{
   DECLARE_ALLOCATOR( RTSPDynamic, AM_DEFAULT );

private:
   Renderer&         m_renderer;
   float             m_widthScale;
   float             m_heightScale;

   RenderTarget*     m_hostTarget;

public:
   RTSPDynamic( Renderer& renderer, float widthScale = 1.f, float heightScale = 1.f );
   ~RTSPDynamic();

   // -------------------------------------------------------------------------
   // RenderTargetSizePolicy implementation
   // -------------------------------------------------------------------------
   void initialize( RenderTarget& target );

   // -------------------------------------------------------------------------
   // Renderer observer implementation
   // -------------------------------------------------------------------------
   void update(Renderer& renderer);
   void update(Renderer& renderer, const RendererOps& operation);
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Defines a single size for the render target.
 */
class RTSPStatic : public RenderTargetSizePolicy
{
   DECLARE_ALLOCATOR( RTSPStatic, AM_DEFAULT );

private:
   unsigned int      m_width;
   unsigned int      m_height;

public:
   RTSPStatic( unsigned int width, unsigned int height );

   // -------------------------------------------------------------------------
   // RenderTargetSizePolicy implementation
   // -------------------------------------------------------------------------
   void initialize( RenderTarget& target );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A dynamic render target policy that matches the size to the size 
 * of another texture.
 */
class RTSPTexture : public RenderTargetSizePolicy, public Observer< ShaderTexture, ShaderTextureOps >
{
   DECLARE_ALLOCATOR( RTSPTexture, AM_DEFAULT );

private:
   Renderer&         m_renderer;
   ShaderTexture&    m_texture;
   RenderTarget*     m_hostTarget;

public:
   /**
    * Constructor.
    *
    * @param renderer   renderer on which the render target should be implemented
    * @param texture    tracked texture
    */
   RTSPTexture( Renderer& renderer, ShaderTexture& texture );
   ~RTSPTexture();

   // -------------------------------------------------------------------------
   // RenderTargetSizePolicy implementation
   // -------------------------------------------------------------------------
   void initialize( RenderTarget& target );

   // -------------------------------------------------------------------------
   // Renderer observer implementation
   // -------------------------------------------------------------------------
   void update( ShaderTexture& texture );
   void update( ShaderTexture& texture, const ShaderTextureOps& operation );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that sets a render target for rendering.
 */
class RCActivateRenderTarget  : public RenderCommand
{
   DECLARE_ALLOCATOR( RCActivateRenderTarget, AM_DEFAULT );

private:
   RenderTarget*     m_renderTarget;
   uint              m_targetIdx;

public:
   /**
    * Constructor.
    *
    * @param renderTarget     ptr to the render target ( NULL value also accepted )
    * @param targetIdx        target index
    */
   RCActivateRenderTarget( RenderTarget* renderTarget, uint targetIdx = 0 );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that unbinds a render target, disabling any further rendering to it.
 */
class RCDeactivateRenderTarget  : public RenderCommand
{
   DECLARE_ALLOCATOR( RCDeactivateRenderTarget, AM_DEFAULT );

private:
   uint              m_targetIdx;

public:
   /**
    * Constructor.
    *
    * @param targetIdx        target index
    */
   RCDeactivateRenderTarget( uint targetIdx = 0 );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Command that allows you to query for the specified pixel color.
 */
class RCGetPixel : public RenderCommand
{
   DECLARE_ALLOCATOR( RCGetPixel, AM_DEFAULT );

private:
   RenderTarget&        m_renderTarget;
   Vector               m_queryPos;
   Color&              m_outColorVal;

public:
   /**
    * Constructor.
    *
    * @param renderTarget        queried render target
    * @param queryPos            pixel position
    * @param outColorVal         this is where the queried color value will be stored
    */
   RCGetPixel( RenderTarget& renderTarget, const Vector& queryPos, Color& outColorVal );

   // -------------------------------------------------------------------------
   // RenderCommand implementation
   // -------------------------------------------------------------------------
   void execute( Renderer& renderer );
};

///////////////////////////////////////////////////////////////////////////////
