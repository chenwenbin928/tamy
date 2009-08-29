#include "impl-DirectX\TTFont.h"


///////////////////////////////////////////////////////////////////////////////

TTFont::TTFont(const char* fontFaceName, 
               const char* fontName, 
               const Color& color)
      : Font(fontName)
{
   // TODO:
}

///////////////////////////////////////////////////////////////////////////////

AbstractGraphicalEntity& TTFont::getChar(unsigned char c)
{
   if (m_glifs[c] == NULL) 
   {
      return *(m_glifs[255]);
   }
   else
   {
      return *(m_glifs[c]);
   }
}

///////////////////////////////////////////////////////////////////////////////

float TTFont::getCharWidth(unsigned char c) const
{
   if (m_glifs[c] == NULL) 
   {
      return m_glifSizes[255];
   }
   else
   {
      return m_glifSizes[c];
   }
}

///////////////////////////////////////////////////////////////////////////////

const Material& TTFont::getMaterial() const
{
   // TODO:
   return *(reinterpret_cast<Material*>(NULL));
}

///////////////////////////////////////////////////////////////////////////////
