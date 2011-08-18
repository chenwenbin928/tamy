/// @file   dx9-Renderer/DX9FontResourcesStorage.h
/// @brief  storage for font implementations
#pragma once

#include "core-Renderer\RenderResourceStorage.h"
#include "core-Renderer\Font.h"
#include <d3d9.h>
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

class DX9Renderer;

///////////////////////////////////////////////////////////////////////////////

typedef RenderResourceStorage< DX9Renderer, Font, ID3DXFont > FontStorage;

///////////////////////////////////////////////////////////////////////////////
