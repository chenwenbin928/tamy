# Memory allocators #

Sometimes we want to place objects in an area of memory of someone elses choosing.

This can be achieved by using a memory allocator.

## Allocator interface ##
There is no explicit allocator interface definition present in the enginem
and that's because memory operations are:
  * performance-crucial
  * need to be supported across different platfors
  * are at the lowest level of all calls

... and for these reasons we simply can't use inheritance.
Rather, we're establishing a common interface by contract.

The contracted interface consists of the following methods
```
/**
 * Destroys an object allocated by the allocator
 */
template< typename T >
void destroy( T* obj );

/**
 * Allocates a chunk of memory of the specified size.
 *
 * @param size
 */
void* alloc( size_t size );

/**
 * Deallocates the memory from the specified address.
 */
void dealloc( void* ptr );
```

## Existing allocators ##

  * `DefaultAllocator` - allocates memory in the system heap
  * `MemoryPoolAllocator` - allocates memory in a dedicated MemoryPool

## Allocated object's placement operators ##

In order to be able to allocate an object using a custom allocator,
you need to provide it with placement operator definitions.

Here's how it's done:

**HEADER**
```
class SomeClass
{
public:
   // ------------------------------------------------------------------
   // Placement allocation operators
   // ------------------------------------------------------------------
   void* operator new( size_t size, TAllocator& allocator );
   void operator delete( void* ptr, TAllocator& allocator );

private:
   // ------------------------------------------------------------------
   // Disabled regular allocation operators
   // ------------------------------------------------------------------
   void* operator new( size_t size );
   void operator delete( void* ptr );
};
```

**IMPLEMENTATION**
```
void* SomeClass::operator new( size_t size, TAllocator& allocator )
{
   return allocator.alloc( size );
}

void SomeClass::operator delete( void* ptr, TAllocator& allocator )
{
   allocator.dealloc( ptr );
}

void* SomeClass::operator new( size_t size )
{
   ASSERT_MSG( false, "Linked list element can only be allocated using a dedicated allocator." );
   return NULL;
}

void SomeClass::operator delete( void* ptr )
{
   ASSERT_MSG( false, "Linked list element can only be allocated using a dedicated allocator." );
}
```


And you can simply use it like so:
```
CustomAllocator allocator;
SomeClass* instance = new ( allocator ) SomeClass();
```

## Memory leaks tracing ##
To enable memory leaks tracing, simply uncomment the `_TRACK_MEMORY_ALLOCATIONS` define located in core/EngineDefines.h.

Memory tracker uses two helper classes to caquire and store callstack traces:

  * `CallstackTracer` - is a platform dependent class that resolves current callstacks and provides a toolset to decipher them into a human readable form
  * `CallstackTree` - stores callstacks acquired using `CallstackTracer` in a memory efficient way.


The system traces all memory allocations that go through the global `MemoryRouter` instance.