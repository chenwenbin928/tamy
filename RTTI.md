# RTTI #

Engine's runtime type identification system.

It's acutally a little bit more than that - it supports objects with properties, their serialization etc.

But let's start slow :)

## Reflection type ##

There are two types of RTTI object:

  * `ReflectionType` contains a runtime type information ( and represents primitive or external types )

  * `SerializableReflectionType` is a subclass of the former and extends it with the serialization capabilities. Keep in mind that you can define this kind of type ONLY for the classes that inherit after `ReflectionObject`.

This dualism is required, because we want to be able to reflect upon external and primitive types ( such as `int` or `D3DXMATRIX` ), which we simply cannot extend with the required type info data.


So what is this `ReflectionObject` - it's a marker interface, that's all. You just need to derive from it and use one of our handy macros to add a type definitions to it:

```
```

## Reflection type components ##

These components represent the member fields of a particular class or a structure, and can be defined only for a `SerializableReflectionType`.

You can define it for a given type using this method of the `SerializableReflectionType` object:

```
/**
 * Adds a new type member field.
 *
 * @param member     type member field description
 */
void addMemberField( ReflectionTypeComponent* member );
```

and here's an example how it may be done:
```

struct TestClass : public ReflectionObject
{
   int m_val;
};

// ...

SerializableReflectionType type( "TestClass" );

TestClass nullObj;
type.addMemberField( new TMemberField< int >( "m_val", (void*)&nullInstance, (void*)&nullInstance.m_val) );

```

So basically what happens here is - we have a template object `nullObj` of class `TestClass` and a type that represents it, and this class has a member - an `int` field named `m_val`.

So we need to query the address of the field, the address of the object it belongs to and pass that info to the `TMemberField` class constructor, specifying the type of the field as its template parameter.
`TMemberField` class instance will then memorize the data layout where the field is located in a given `TestClass` instance and will know how to get to the data that's stored there.


## Types registry ##

All classes are registered at program startup with a global types registry - `ReflectionTypesRegistry`.

It contains an API useful for finding types by either their handle ( which can be obtained using `ReflectionType::generateId` method ) or by the C++ type using a template version of the `find` method:

```
/**
 * Looks for a type with the specified id.
 */
template< typename T >
ReflectionType* find() const;

/**
 * Looks for a type with the specified id.
 *
 * @param id
 */
ReflectionType* find( unsigned int id ) const;

/**
 * Looks for a serializable type with the specified id.
 * 
 * @param id
 */
SerializableReflectionType* findSerializable( unsigned int id ) const;
```

## Properties ##

Properties are a runtime version of the member fields. The latter are used as a a part of the serializable definition in the serialization process, and the latter are their runtime version that allows to access the data, edit it using externally created editors etc.

## Runtime type identification features ##

Given an object or its name, you can perform various operations:

  * check if it exactly matches the specified type

```
```

  * check if the specified reference type is one of the types from the inheritance hierarchy tree of the checked type

```
```

  * down/up-cast an obejct to the specified type

```
```