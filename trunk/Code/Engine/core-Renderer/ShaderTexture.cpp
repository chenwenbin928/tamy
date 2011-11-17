#include "core-Renderer/ShaderTexture.h"

///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( TextureUsage );
   ENUM_VAL( TU_COLOR );
   ENUM_VAL( TU_DEPTH );
   ENUM_VAL( TU_LUMINANCE );
   ENUM_VAL( TU_HDR );
   ENUM_VAL( TU_DATA_TEXTURE );
END_ENUM( TextureUsage );

///////////////////////////////////////////////////////////////////////////////
