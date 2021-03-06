#ifndef _MEMORY_ROUTER_H
#error "This file can be included only in MemoryRouter.h"
#else

#include <memory>
#include "core\MemoryUtils.h"


///////////////////////////////////////////////////////////////////////////////

template< typename T >
void MemoryRouter::destroy( T* obj, AllocationMode allocMode, MemoryAllocator* allocator )
{
   if ( obj )
   {
      obj->~T();
      dealloc( obj, allocMode, allocator );
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _MEMORY_ROUTER_H
