#include "QtWindowBuilder.h"
#include "TamySceneWidget.h"


///////////////////////////////////////////////////////////////////////////////

QtWindowBuilder::QtWindowBuilder(TamySceneWidget& window)
: m_window(window)
{
}

///////////////////////////////////////////////////////////////////////////////

HWND QtWindowBuilder::create(IWindowMessagesProcessor& winMsgProcessor)
{
   m_window.setWinMsgProcessor(winMsgProcessor);
   return m_window.getWinHandle();
}

///////////////////////////////////////////////////////////////////////////////