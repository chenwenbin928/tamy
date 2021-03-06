/// @file   core/DefaultAllocator.h
/// @brief  default memory allocator
#pragma once

#include "core\types.h"
#include "core\MemoryAllocator.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * Default memory allocator.
 */
class DefaultAllocator : public MemoryAllocator
{
private:
   ulong       m_allocatedMemorySize;

public:
   DefaultAllocator();

   // -------------------------------------------------------------------------
   // MemoryAllocator implementation
   // -------------------------------------------------------------------------
   void* alloc( size_t size );
   void dealloc( void* ptr );
   ulong getMemoryUsed() const { return m_allocatedMemorySize; }
};

///////////////////////////////////////////////////////////////////////////////
