#pragma once

#include "ext-UI\UIWidget.h"


///////////////////////////////////////////////////////////////////////////////

class Node;

///////////////////////////////////////////////////////////////////////////////

class Button : public UIWidget
{
private:
   Node* m_avatar;

public:
   Button(Node* avatar);
};

///////////////////////////////////////////////////////////////////////////////
