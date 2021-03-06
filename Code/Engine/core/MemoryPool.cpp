#include "core.h"
#include "core/MemoryPool.h"
#include "core/MemoryUtils.h"
#include "core/Assert.h"
#include <stdio.h>


///////////////////////////////////////////////////////////////////////////////

int MemoryPool::POOL_INFO_CHUNK_SIZE = sizeof( size_t ) + sizeof( void* );

///////////////////////////////////////////////////////////////////////////////

MemoryPool::MemoryPool( size_t size )
{
   m_bufSize = size;
   m_memory = new char[ m_bufSize ];
   m_allocStartOffset = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

MemoryPool::~MemoryPool()
{
   delete [] m_memory;
   m_memory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

size_t MemoryPool::getMemoryUsed() const 
{ 
   return m_allocStartOffset; 
}

///////////////////////////////////////////////////////////////////////////////

void MemoryPool::reset()
{
   ASSERT_MSG( m_allocationsCount == 0, "There are still some objects left in the pool" );

   m_allocStartOffset = 0;
   m_allocationsCount = 0;
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryPool::alloc( size_t size )
{
   size_t additionalAllocationSize = POOL_INFO_CHUNK_SIZE;
   size_t sizeAtEnd = m_bufSize - m_allocStartOffset - additionalAllocationSize;

   if ( size < sizeAtEnd )
   {
      // get the pointer to the allocated memory chunk
      char* ptr = m_memory + m_allocStartOffset;

      // memorize pointer to the buffer and the chunk size
      size += additionalAllocationSize;
      *(void**)ptr = this;
      ptr += sizeof( void* );
      *(size_t*)ptr = size;
      ptr += sizeof( size );

      // move the pointer of the next free memory area
      m_allocStartOffset += size;
      ++m_allocationsCount;

      // return a pointer to the allocated memory
      return ptr;
   }
   else
   {
      // there's not enough memory
      ASSERT_MSG( false, "Not enough memory in the memory pool" );
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void MemoryPool::dealloc( void* ptr )
{
   --m_allocationsCount;

   if ( m_allocationsCount == 0 )
   {
      // reset the allocation start offset
      m_allocStartOffset = 0;
   }
}

///////////////////////////////////////////////////////////////////////////////

MemoryPool* MemoryPool::getPoolPtr( void* ptr )
{
   MemoryPool* poolPtr = reinterpret_cast< MemoryPool* >( *(void**)( (char*)ptr - POOL_INFO_CHUNK_SIZE ) );
   if ( !poolPtr )
   {
      ASSERT_MSG( false, "Accessing an object that's not allocated in a memory pool" );
   }
   return poolPtr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

MemoryPoolObject::~MemoryPoolObject()
{
   // get the memory
   MemoryPool* poolPtr = MemoryPool::getPoolPtr( this );
   if ( poolPtr )
   {
      poolPtr->dealloc( this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryPoolObject::operator new( size_t size, MemoryPool& pool )
{
   return pool.alloc( size );
}

///////////////////////////////////////////////////////////////////////////////

void MemoryPoolObject::operator delete( void* ptr, MemoryPool& pool )
{
   pool.dealloc( ptr );
}

///////////////////////////////////////////////////////////////////////////////

void* MemoryPoolObject::operator new( size_t size )
{
   ASSERT_MSG( false, "Round buffer object can only be allocated in a round buffer." );
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void MemoryPoolObject::operator delete( void* ptr )
{
   ASSERT_MSG( false, "Can't delete a round buffer object - you need to delete the buffer that holds it." );
}

///////////////////////////////////////////////////////////////////////////////
