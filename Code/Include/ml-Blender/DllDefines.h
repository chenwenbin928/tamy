/// @file   ml-Blender/DllDefines.h
/// @brief  dll lib definitions
#pragma once


///////////////////////////////////////////////////////////////////////////////

#ifdef ML_BLENDER_EXPORTS
#define ML_BLENDER_API extern "C" __declspec(dllexport)
#else
#define ML_BLENDER_API extern "C" __declspec(dllimport)
#endif

///////////////////////////////////////////////////////////////////////////////
