#include "core-TestFramework\TestFramework.h"
#include "core/RoundBuffer.h"
#include "core\Timer.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class RoundBufferObjectMock : public RoundBufferObject
   {
   };

} // anonymous

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, oneObject )
{
   RoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObject* obj = new( buffer ) RoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj, buffer.front< RoundBufferObject >() );

   ROUNDBUF_DELETE( obj );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( (RoundBufferObject*)NULL, buffer.front< RoundBufferObject >() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, twoObjects )
{
   RoundBuffer buffer( 512 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObject* obj1 = new( buffer ) RoundBufferObjectMock();
   RoundBufferObject* obj2 = new( buffer ) RoundBufferObjectMock();

   CPPUNIT_ASSERT_EQUAL( (unsigned int)2, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj1, buffer.front< RoundBufferObject >() );

   ROUNDBUF_DELETE( obj1 );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)1, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( obj2, buffer.front< RoundBufferObject >() );

   ROUNDBUF_DELETE( obj2 );
   CPPUNIT_ASSERT_EQUAL( (unsigned int)0, buffer.getAllocationsCount() );
   CPPUNIT_ASSERT_EQUAL( (size_t)0, buffer.getMemoryUsed() );
   CPPUNIT_ASSERT_EQUAL( (RoundBufferObject*)NULL, buffer.front< RoundBufferObject >() );
}

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, manyAllocations )
{
   RoundBuffer buffer( 4096 );

   const int OBJECTS_COUNT = 100;
   RoundBufferObject* arr[ OBJECTS_COUNT ];

   // allocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      arr[j] = new( buffer ) RoundBufferObjectMock();
      CPPUNIT_ASSERT_EQUAL( arr[0], buffer.front< RoundBufferObject >() );
   }

   // deallocate
   for ( int j = 0; j < OBJECTS_COUNT; ++j )
   {
      ROUNDBUF_DELETE( arr[j] );
      if ( j < OBJECTS_COUNT - 1 )
      {
         CPPUNIT_ASSERT_EQUAL( arr[ j + 1 ], buffer.front< RoundBufferObject >() );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

TEST( RoundBuffer, performance )
{
   RoundBuffer buffer( 4096 );

   const int OBJECTS_COUNT = 100;
   const int LOOPS_COUNT = 65535;
   RoundBufferObject* arr[ OBJECTS_COUNT ];

   CTimer timer;
   timer.tick();
   // check the execution time of an empty loop
   for ( int i = 0; i < LOOPS_COUNT; ++i ) {}
   timer.tick();
   float emptyLoopDuration = timer.getTimeElapsed();

   // now measure the time of multiple allocations / deallocations
   timer.tick();
   for ( int i = 0; i < LOOPS_COUNT; ++i )
   {
      // allocate
      for ( int j = 0; j < OBJECTS_COUNT; ++j )
      {
         arr[j] = new( buffer ) RoundBufferObjectMock();
      }

      // deallocate
      for ( int j = 0; j < OBJECTS_COUNT; ++j )
      {
         ROUNDBUF_DELETE( arr[j] );
      }
   }
   timer.tick();
   float allocDeallocDuration = timer.getTimeElapsed();
   CPPUNIT_ASSERT( 2.0f >= allocDeallocDuration );
}

///////////////////////////////////////////////////////////////////////////////
