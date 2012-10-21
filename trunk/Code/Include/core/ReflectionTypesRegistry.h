/// @file   core/ReflectionTypesRegistry.h
/// @brief  a registry of reflection types
#ifndef _REFLECTION_TYPES_REGISTRY_H
#define _REFLECTION_TYPES_REGISTRY_H

#include <map>
#include <vector>
#include <core\types.h>


///////////////////////////////////////////////////////////////////////////////

class ReflectionType;
class SerializableReflectionType;
struct SerializableTypeInstantiator;
class MemoryPool;
class MemoryPoolAllocator;

///////////////////////////////////////////////////////////////////////////////

/**
 * A registry of reflection types.
 */
class ReflectionTypesRegistry
{
public:
   MemoryPoolAllocator*                                     m_sharedMemoryPoolAllocator;

   // This memory pool can be accessed using the exposed public allocator - m_sharedMemoryPoolAllocator.
   // It's main purpose is to provide a common, dedicated area in memory for all type-related
   // functionality in order to avoid constant memory reallocations
   MemoryPool*                                              m_sharedMemoryPool;

private:
   typedef std::map< int, ReflectionType* >                 BaseTypesMap;
   typedef std::map< int, SerializableReflectionType* >     SerializableTypesMap;

   BaseTypesMap                                             m_externalTypesMap;
   SerializableTypesMap                                     m_serializableTypesMap;
   std::vector< ReflectionType* >                           m_allTypes;
   ReflectionType*                                          m_genericEnumType;

private:
   static ReflectionTypesRegistry*                          s_theInstance;

public:
   /**
    * Constructor.
    */
   ReflectionTypesRegistry();
   ~ReflectionTypesRegistry();

   /**
    * Adds a new type to the registry. This type HAS TO inherit after ReflectionObject.
    *
    * @param T                   type to add
    * @param typeName            name of the type
    * @param instantiator        an instantiator that can create an instance of the class. Specify NULL to make the type abstract.
    * @param patchedTypeName     name of the type we're patching with this type
    * @return                    'true' if the type was added successfully, 'false' otherwise
    */
   template< typename T >
   bool addSerializableType( const std::string& typeName, SerializableTypeInstantiator* instantiator, const std::string& patchedTypeName = "" );

   /**
    * Adds a new type that doesn't have to inherit after ReflectionObject ( which is mostly
    * the case with types external to the engine, like DirectX types for instance, that
    * we still want to reflect upon )
    *
    * @param T                   type to add
    * @param typeName            name of the type
    * @return                    'true' if the type was added successfully, 'false' otherwise
    */
   template< typename T >
   bool addExternalType( const std::string& typeName );

   /**
    * Adds a new type enumerated type.
    *
    * @param T                   type to add
    * @param typeName            name of the type
    * @return                    'true' if the type was added successfully, 'false' otherwise
    */
   template< typename T >
   bool addEnumType( const std::string& typeName );

   /**
    * Returns a type definition corresponding to the generic enum type.
    */
   inline ReflectionType& getGenericEnumType() const;

   /**
    * Looks for a type with the specified id.
    *
    * @param id
    */
   template< typename T >
   ReflectionType* find() const;
   ReflectionType* find( unsigned int id ) const;
   ReflectionType* find( const std::string& typeName ) const;

   /**
    * Looks for a serializable type with the specified id.
    * 
    * @param id
    */
   SerializableReflectionType* findSerializable( unsigned int id ) const;
   SerializableReflectionType* findSerializable( const std::string& typeName ) const;

   /**
    * Looks for types that either are the type specified as the template
    * parameter, or derive from it, and put them in the specified array.
    */
   template< typename T >
   void getMatchingTypes( std::vector< const ReflectionType* >& outTypes ) const;

   /**
    * Looks for serializable types that either are the type specified as the template
    * parameter, or derive from it, and put them in the specified array.
    *
    * @param outTypes                  the types will be appended to this list
    * @param includeAbstractTypes      should abstract types be listed as well?
    */
   template< typename T >
   void getMatchingSerializableTypes( std::vector< const SerializableReflectionType* >& outTypes, bool includeAbstractTypes ) const;
   void getMatchingSerializableTypes( uint id, std::vector< const SerializableReflectionType* >& outTypes, bool includeAbstractTypes ) const;

   /**
    * Flushes all registered types.
    */
   void clear();

   /**
    * Returns the singleton instance of the class.
    */
   static inline ReflectionTypesRegistry& getInstance();
};

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionTypesRegistry.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_TYPES_REGISTRY_H