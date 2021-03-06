#include "core-Renderer\Font.h"



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

RCDrawText::RCDrawText( Font& font, const std::string& text, const RECT& overlaySize, TextJustification justification, const Color& color )
   : m_font( font )
   , m_text( text )
   , m_overlaySize( overlaySize )
   , m_justification( justification )
   , m_color( color )
{
}

///////////////////////////////////////////////////////////////////////////////
