# Round Buffer #

A round buffer is a type of memory pool and acts like a queue objects can be added to and then executed in the same order they were added in.

As an example, see the [rendering commands queue](Renderer#CommandsQueue.md) - it's build on this very mechanism.

## Usage ##

That's easy - just create a `RoundBuffer object`, and start allocating RoundBufferObjects in it:

```
#include "core/RoundBuffer.h"

// .....

RoundBuffer buffer( maxObjectsCount );

// add a few objects
new ( buffer ) MyPoolObject( 1 );
new ( buffer ) MyPoolObject( 2 );
new ( buffer ) MyPoolObject( 3 );

// read them back
MyPoolObject* obj = NULL;
while ( c = m_renderCommands.front< MyPoolObject>() )
{
   // we're done - delete the object
   ROUNDBUF_DELETE( c );
}

```

First thing to notice is the number you pass in the buffer's constructor. It's the buffer's capacity in bytes.
We need to specify this, because the buffer is a memory pool - a constant and continuous memory block, and we need to describe how large it is.

Mind that you need to calculate this size based on the size and max amount of objects you want to store in the buffer at the same time.


Reading them back is also simple - just keep calling the `front` method until it returns NULL.

Once you're done with the retrieved object, be sure to release its resources ( the memory will be managed by the buffer, so don't worry ), by calling the handy `ROUNDBUF_DELETE` macro.


## Objects ##
The buffer can accomodate anything that implements `RoundBufferObject` interface, so simply extend it and you're good to go.

It doesn't burden you with any methods that require implementing or reimplementing.

Here's an example:

```
class MyPoolObject : public RoundBufferObject
{
}
```