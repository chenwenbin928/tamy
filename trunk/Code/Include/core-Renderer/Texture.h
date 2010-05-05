#pragma once

/// @file   core-Renderer\Texture.h
/// @brief  image rendered on a surface

#include "core\Resource.h"
#include "core\UniqueObject.h"
#include "core-Renderer\RendererObject.h"
#include "core-Renderer\RendererObjectImpl.h"
#include <string>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

class Filesystem;
class SingletonsManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * Library dependent texture implementation.
 */
class TextureImpl : public RendererObjectImpl
{
public:
   virtual ~TextureImpl() {}

   /**
    * Should return a pointer to the library-specific texture representation.
    *
    * @return     pointer to impl-specific texture structure
    */
   virtual void* getPlatformSpecific() const {return NULL;}
};

///////////////////////////////////////////////////////////////////////////////

/**
 * An image that can be drawn on a rendered surface.
 *
 * CAUTION: If I want to create a texture render target,
 * I need to:
 *   1.) create a separate class RenderTarget
 *   2.) rename this one to an ImageTexture (and its implementation
 *       to ImageTextureImpl)
 *   3.) Create a common Texture interface for both that will allow
 *       them to operate with Shaders etc.
 */
class Texture : public TRendererObject<TextureImpl>, 
                public UniqueObject, 
                public Resource
{
   DECLARE_RESOURCE( Texture )

private:
   std::string m_texFileName;
   
   byte* m_imgBuffer;
   unsigned int m_bufSize;

public:
   /**
    * Constructor.
    */
   Texture( const std::string& fileName = "" );
   ~Texture();

   /**
    * Returns the name of th eimage file.
    */
   inline const std::string& getTextureName() const { return m_texFileName; }

   /**
    * Returns a pointer to the platform specific implementation
    * of the texture.
    *
    * Only the implementation will know what to do with it.
    *
    * @return     pointer to impl-specific texture structure
    */
   void* getPlatformSpecific() const;

   /**
    * A texture is just an image. This method returns a pointer
    * to the memory buffer occupied by the image.
    *
    * @param outSize    output parameter that will be initialized
    *                   with the buffer size (in bytes)
    * @param outBuffer  output parameter that will be initialized
    *                   with a pointer to the image memory buffer
    */
   void getBuffer(unsigned int& outSize, byte** outBuffer);

   /**
    * The method releases the image buffer when it's no longer needed.
    */
   void releaseData();

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void onResourceLoaded(ResourcesManager& mgr);

private:
   void loadFromFile( const Filesystem& fs );
};

///////////////////////////////////////////////////////////////////////////////
