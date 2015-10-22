# Pipeline widget #

It's not as much a widget, as it is a set of skeleton classes that facilitate the creation of pipeline layouts.


I'll give a quick example of what do you have to create in order to quickly create a layout for your pipeline resource ( the pipeline classes match those we defined in the [this example](DataPipeline#How_to_use_it.md) ).


**First**, you need a layout definition:

  * .h file
```
/// @file   TamyEditor/MyPipelineLayout.h
/// @brief  my pipeline layout
#pragma once

#include "GraphLayout.h"
#include "PipelineLayout.h"


////////////////////////////////////////////////////////////////////////

class MaterialNode;
class PipelineBlock;

////////////////////////////////////////////////////////////////////////

/**
 * Layout for your pipeline.
 */
class MyPipelineLayout : public PipelineLayout< MyGraphBuilder, MyGraphNode >
{
   DECLARE_RESOURCE( MyPipelineLayout )

public:
   /**
    * Constructor.
    *
    * @param resourceName
    */
   MyPipelineLayout( const FilePath& resourceName = FilePath() );

protected:
   // ------------------------------------------------------------------
   // Resource implementation
   // ------------------------------------------------------------------
   void initBlocksFactory();
   void initSocketsFactory( PipelineBlock& block );
};

////////////////////////////////////////////////////////////////////////

```

  * .cpp file

```
#include "MyPipelineLayout.h"


// blocks & sockets
// add your node and socket header files includes here


////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( MyPipelineLayout, Resource, rmpl, AM_BINARY );
   PROPERTY( MyGraphBuilder*, m_pipeline );
   PROPERTY( std::vector< GraphBlock* >, m_blocks );
   PROPERTY( std::vector< GraphBlockConnection* >, m_connections );
   // custom properties definitions go here
END_RESOURCE();

////////////////////////////////////////////////////////////////////////

MyPipelineLayout::MyPipelineLayout( const FilePath& resourceName )
   : PipelineLayout< MyGraphBuilder, MyGraphNode >( resourceName )
{
   // create block-to-node associations
   initBlocksFactory();

   // customize the looks
   setBackgroundBrush( QColor( 209, 226, 165 ) );
}

////////////////////////////////////////////////////////////////////////

void MaterialLayout::initBlocksFactory()
{
   // add your node-to-block associations here
   associate< MyGraphActionNode, MyGraphActionBlock >();
}

////////////////////////////////////////////////////////////////////////

void MaterialLayout::initSocketsFactory( PipelineBlock& block )
{
   GenericFactory< GBNodeSocket, GraphBlockSocket >& factory = block.getSocketsFactory();

   // add sockets definitions here
   factory.associate< MyFloatInput, MyFloatSocket >();
   factory.associate< MyFloatOutput, MyFloatSocket >();
}

////////////////////////////////////////////////////////////////////////
```

> `MyGraphActionNode` is a specialisation of `MyGraphNode` that actually does something in the pipeline, and `MyGraphActionBlock` will be a block representing it ( we'll define it down the road )

  * type definitions

```
REGISTER_RTTI( MyPipelineLayout );
```


**Next**, you need to define sockets ( we're using only one in this example - `MyFloatSocket` - which is used to represent both `MyFloatInput` and  `MyFloatOutput`, and we're gonna stick with that ):

  * .h file
```
/// @file   TamyEditor/MyPipelineSockets.h
/// @brief  mypipeline's socket representation
#pragma once

#include "PipelineSocket.h"


///////////////////////////////////////////////////////////////////////

DECLARE_PIPELINE_SOCKET( MyFloatSocket, MyGraphNode, 163, 255, 247 );

///////////////////////////////////////////////////////////////////////

```

  * .cpp file
```
#include "MyPipelineSockets.h"


////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MyFloatSocket, GraphBlockSocket );
END_OBJECT();

////////////////////////////////////////////////////////////////////////
```

  * type definitions

```
REGISTER_RTTI( MyFloatSocket );
```


**And finally** define the blocks that will represent particular nodes.

  * .h file
```
/// @file   TamyEditor/MyGraphActionBlock .h
/// @brief  my pipeline block
#pragma once


#include "PipelineBlock.h"
#include "core/ResourceHandle.h"


////////////////////////////////////////////////////////////////////////

/**
 * My pipeline block.
 */
class MyGraphActionBlock : public TPipelineBlock< MyGraphActionNode, MyGraphNode >
{
   DECLARE_CLASS( MyGraphActionBlock)

public:
   /**
    * Default constructor required by the RTTI system.
    */
   MyGraphActionBlock() : TPipelineBlock< MyGraphActionNode, MyGraphNode >() {}

   /**
    * Parametrized constructor required by the generic factory.
    *
    * @param node    represented node
    */
   MyGraphActionBlock( MyGraphActionNode& node );

protected:
   // ------------------------------------------------------------------
   // GraphBlock implementation
   // ------------------------------------------------------------------
   /**
    * Returns the shape of the block.
    */
   virtual Shape getShape() const { return GBS_ROUNDED; }
   virtual QColor getBgColor() const { return QColor( 141, 136, 167 ); }
};

////////////////////////////////////////////////////////////////////////
```

  * .cpp file

```
#include "MyGraphActionBlock.h"


////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MyGraphActionBlock, GraphBlock );
   PROPERTY( TResourceHandle< MyGraphActionNode>*, m_node );
END_OBJECT();

////////////////////////////////////////////////////////////////////////

MyGraphActionBlock::MyGraphActionBlock( MyGraphActionNode& node )
   : TPipelineBlock< MyGraphActionNode, MyGraphNode >( node )
{
   setCaption( "Action" );
}

////////////////////////////////////////////////////////////////////////

```

  * type definitions

```
REGISTER_RTTI( MyGraphActionBlock );
```