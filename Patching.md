# Patching #

Very often you will require to change definitions of certain types - the code evolves, it's being refactored - that's just how it is.

The problem is that you may be loading data from your resource files that reference those types.
How should the reflection system know how to load them then, since the definition has changed?

Patching to the rescue!

## PatchesDB ##
`PatchesDB` is a singleton construct that stores differential definitions of how particular types have changed from one version to another.

You can add such a definition using database's API, or by storing it in an XML file ( this will be described in details in a dedicated section later on ).


A typical patch definition for a given type should always start from the initial version of the type. Each subsequent entry increases type's version and describes what aspect of the type has changed.

Let's walk through an example showing how to describe patching information step by step, as a sample type migrates from one form to another:

### Initial version ###

Let's start with a simple class
```
class MyClass : public ReflectionObject
{
   DECLARE_CLASS();

public:
   int    m_intField;
};

BEGIN_OBJECT( MyClass );
   PARENT( ReflectionObject );
   PROPERTY( int, m_intField );
END_OBJECT();
```

In this state we don't need to define any patching information - the class is at it's version 0 ( which is implicitly defined ), and a proper patching information will be generated for it automatically, provided you include the following method call in your application's startup sequence:

```
PatchesDB& patchesDB = PatchesDB::getInstance();
ReflectionTypesRegistry::getInstance().build( patchesDB );
```


But just for the sake of it, let's see how such a definition would look like.
Here's the code:

```
PatchesDB& patchesDB = PatchesDB::getInstance();

PatchRecord& record = patchesDB.addPatch( "", -1, "MyClass", 0 );
record.addParent( "ReflectionObject", 0 );
record.addField( "m_intField", SFT_Value );
```

What happen's here, step by step, is:
  * first we create a new patch record, which defines 'an upgrade' from a non-existing version -1 to version 0 for a type called "`MyClass`"
  * next, we define its parent as "`ReflectionObject`". This particular type depends on version 0 of that type.
  * last, we add a field called "m\_intField", that is a value field

Let's discuss two important concepts here:
  * _version of the parent type_ - a particular definition needs to specify which version of a type it was parented by exactly. If that type has changed as well, the system would need to know what version to start the patching from.
  * _member field type_ - there are four options available here:
    * `SFT_Value` - refers to regular members of a primitive or complex types ( int, float, std::string, Vector etc. )
    * `SFT_Pointer` - refers to all pointers
    * `SFT_ArrayOfValues` - refers to std::vector and Array instances of primitive or complex types
    * `SFT_ArrayOfPointers` - refers to std::vector and Array instances of pointers

### Version 1 - changing name of the field and the name of the type ###

Say that for some reason we don't like the name of both the field an the type - maybe it was not expressive enough etc...

So we're redefining the class like so:

```
class Counter : public ReflectionObject
{
   DECLARE_CLASS();

public:
   int    m_counterValue;
};

BEGIN_OBJECT( Counter );
   VERSION( 1 );
   PARENT( ReflectionObject );
   PROPERTY( int, m_counterValue );
END_OBJECT();
```

Notice that we added a version definition to the type's description.
Even though this is the first record of type "`Counter`", we know that it migrated from "`MyClass`".

Here's how we should define a patch record that describes these changes:

```
PatchesDB& patchesDB = PatchesDB::getInstance();

PatchRecord& record = patchesDB.addPatch( "MyClass", 0, "Counter", 1 );
record.changeField( "m_intField", "m_counterValue" );
```

What happens here is:
  * we define a new patch record that increases the version number and also redefines the name of the class
  * we change the name of the field from "`m_intField`" to "`m_counterValue`"

### Changing type of a member field ###

After having worked with the class for a while we realized, that `int` type is just not cut out for our member field "`m_counterValue`" - it's just too constraining.

We need to account for larger numbers now, and we also need to keep a text representation of that value ( for some reason ).

So here's how our class's definition would look like in order to satisfy those requirements:

```
class Counter : public ReflectionObject
{
   DECLARE_CLASS();

public:
   long           m_counterValue;
   std::string    m_strCounterValue;
};

BEGIN_OBJECT( Counter );
   VERSION( 2 );
   PARENT( ReflectionObject );
   PROPERTY( long, m_counterValue );
   PROPERTY( std::string, m_strCounterValue);
END_OBJECT();
```


Now the trick here is - how do we automatically populate the string field when we load old objects?

A [patch function custom patch function](Patching#Custom.md) to the rescue :)

First, let's define the custom patch function:
```
void patchCounter_1_to_2( SerializedReflectionObject& migratedObject )
{
   const SerializedValue* longValue = migratedObject.getValue( "m_counterValue" );
   SerializedValue* strValue = migratedObject.getValue( "m_strCounterValue" );

   long val;
   longValue->initialize< long >( val );

   std::string strVal = itoa( val );
   strValue->set< std::string >( strVal );
}
```

What this function does is it looks up data proxies for the two member fields of our interest, load the value of the one that stores the number, converts it to text using a standar library method `itoa`, and then populates the string field with it.

Now we need to inform the patching system about the new field and the custom function that should be used:

```
PatchesDB& patchesDB = PatchesDB::getInstance();

PatchRecord& record = patchesDB.addPatch( "Counter", 1, "Counter", 2 );
record.addField( "m_strCounterValue", SFT_Value );
record.defineMigrationFunc( &patchCounter_1_to_2);
```

You should be able to understand what this record does by now. The only new thing here is the definition of the custom patch function - we provide a pointer to it.



But wait - didn't we miss something? What about the type of "`m_counterValue`"? It changed from `int` to `long`, didn't it?
So why don't we include that information in the patch record?

That's the beauty of this patching system - it can handle most data types conversions automatically, following standard C++ language rules.
In this case, the value will be carried over witout a loss.
If you would convert data from a less constraining type to a more constraining one ( say from an `int` to a `char` ), you would face some data loss, but that's understandable.

It's the less obvious data coversion cases, such as the example provided here, that require you to take special measures in order to provide meaningful data - and that's what the custom patch function mechanism is for.


## Patch definition XML ##

In the previous paragraph we walked through a valid code migration example and showed how to set up the patches db so that it could handle them correctly.

But the examples showed how you can work with the PatchesDB directly from code, which is not exactly convenient and well readable.

Patch definitions form a database that's not going to change, but is going to grow and should be easily accessible and readable.

So for that purpose, we introduced an XML definition of it.

### Initialization ###
What you need to do is create an XML file that starts with the following tag definition:
```
<?xml version="1.0" encoding="UTF-8"?>
<PatchesDB>

<!-- definition goes here -->

</<PatchesDB>
```

place it somewhere in your assets repository and load it in your application initialization phase:
```
// load patching definitions
Filesystem& fs = ResourcesManager::getInstance().getFilesystem();
FilePath patchesDBDefPath( "/patchesDB.xml" );
File* patchesDBFile = fs.open( patchesDBDefPath, std::ios_base::in );
StreamBuffer< char > fileBuf( *patchesDBFile );
std::string patchesDBContents = fileBuf.getBuffer();
delete patchesDBFile;

PatchesDB& patchesDB = PatchesDB::getInstance();
PatchesDBSerializer::load( patchesDB, patchesDBContents );
```

One **REMARK** - you need to load that definition **BEFORE** you build initial patches definitions for the types currently stored in the types registry with this method call:
```
ReflectionTypesRegistry::getInstance().build( patchesDB );
```

### Version 0 ###

Let's relate to our previous example and show how the corresponding patch definitions would look like in XML:

```
<PatchEntry oldTypeName="" oldVersionNo="-1" newTypeName="MyClass" newVersionNo="0">
	<AddParent type="ReflectionObject" version="0"/>
	<AddField type="SFT_Value" name="m_intField" />
</PatchEntry>
```

### Version 1 ###

```
<PatchEntry oldTypeName="MyClass" oldVersionNo="0" newTypeName="Counter" newVersionNo="1">
	<ChangeField oldName="m_intField" newName="m_counterValue" />
</PatchEntry>
```


### Version 2 ###

```
<PatchEntry oldTypeName="Counter" oldVersionNo="1" newTypeName="Counter" newVersionNo="2">
	<AddField type="SFT_Value" name="m_strCounterValue" />
        <DataMigrationFunction name="patchCounter_1_to_2" />
</PatchEntry>
```

In order to make this last definition work, we need to inform the patching system how can he translate the name of a function to its pointer.

In order to do that, place the following macro directly under the function's closing bracket:

```
REGISTER_PATCH_FUNCTION( patchCounter_1_to_2 );
```

It will take care of registering a translation unit for the function with PatchesDB.

## Custom patch function ##

As you saw before, this function can be used to provide custom initialization code that will take care of migrating data from one form to another.

Here's its basic signature:
```
void patchFunctionName( SerializedReflectionObject& migratedObject )
{
}
```

Look up `SerializedReflectionObject` API to see how you can get access to member fields of a type.

### Migrating data between parent and derived type ###
This unfortunately is not supportyed. The function works with a single type at a time, meanining it doesn';t have access to fields defined in the parent type.

### Function pointer registration ###
In order to be able to reference the function from XML files, you need to register it using the following macro:

```
REGISTER_PATCH_FUNCTION( patchFunctionName );
```

SImply place a call to it below the function's closing bracket, and you're good to go,