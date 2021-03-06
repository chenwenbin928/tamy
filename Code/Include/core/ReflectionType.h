/// @file   core/ReflectionType.h
/// @brief  reflection type definition
#ifndef _REFLECTION_TYPE_H
#define _REFLECTION_TYPE_H

#include <vector>
#include <string>
#include <map>
#include <list>
#include "core\MemoryRouter.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionSaver;
class ReflectionLoader;
class ReflectionTypeComponent;
class ReflectionProperty;
class OutStream;
class InStream;
class MemoryPool;


///////////////////////////////////////////////////////////////////////////////

/**
 * A helper singleton class used to tell whether a serialization process is running
 * at the moment - in a thread-safe manner.
 */
class SerializationFlag
{
private:
   static SerializationFlag s_theInstance;

   // <threading.todo> Thread safety is not yet implemented, since the engine is running on a single thread.
   // As soon as we implement multithreading, make this class has a flag for each running thread ( since
   // loading will be implemented on threads )
   bool    m_serializationInProgress;

public:
   static inline SerializationFlag& getInstance() { return s_theInstance; }

   /**
    * Checks if the serialization is in progress.
    */
   bool isSerializationInProgress();

   /**
    * Flags whether the serialization is in progress.
    *
    * @param inProgress
    */
   void flagSerializationInProgress( bool inProgress );

private:
   /**
    * Constructor.
    */
   SerializationFlag();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Definition of a type that can be recognized at runtime and can serialize
 * objects defined using it.
 */
class ReflectionType
{
   DECLARE_ALLOCATOR( ReflectionType, AM_DEFAULT );

public:
   // type name and its id version
   std::string                                        m_name;
   unsigned int                                       m_id;

public:
   /**
    * Constructor.
    *
    * @param name             type name
    */
   ReflectionType( const std::string& name );
   virtual ~ReflectionType() {}

   /**
    * A utility method for generating a unique ID based on the specified type name.
    *
    * @param typeName
    */
   static inline uint generateId( const std::string& typeName );

   // ----------------------------------------------------------------------
   // Comparison
   // ----------------------------------------------------------------------
   
   /**
    * Checks if this type can be downcast onto the specified reference type.
    *
    * @param referenceType
    */
   virtual bool isA( const ReflectionType& referenceType ) const;

   /**
    * Checks if this type can exactly matches the specified reference type.
    *
    * @param referenceType
    */
   inline bool isExactlyA( const ReflectionType& referenceType ) const;
};

///////////////////////////////////////////////////////////////////////////////

struct SerializableTypeInstantiator
{
   virtual ~SerializableTypeInstantiator() {}

   virtual void* instantiate() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

template< typename T >
struct TSerializableTypeInstantiator : public SerializableTypeInstantiator
{
   DECLARE_ALLOCATOR( TSerializableTypeInstantiator, AM_DEFAULT );

   void* instantiate() const;
};

/**
 * Definition of a type that can be recognized at runtime and can serialize
 * objects defined using it.
 */
class SerializableReflectionType : public ReflectionType
{
   DECLARE_ALLOCATOR( SerializableReflectionType, AM_DEFAULT );

public:
   std::string                                        m_patchedName;
   unsigned int                                       m_patchedId;

   SerializableTypeInstantiator*                      m_instantiator;
   std::vector< ReflectionTypeComponent* >            m_memberFields;

   typedef std::map< uint, uint >                     NamesMap;
   NamesMap                                           m_patchedMemberNames;

   std::vector< uint >                                m_baseTypesIds;

   // runtime data
   MemoryPool*                                        m_searchMemPool;

public:
   /**
    * Constructor.
    *
    * @param name             type name
    * @param patchedName      name of the type this type patches
    */
   SerializableReflectionType( const std::string& name, const std::string& patchedName = "" );
   ~SerializableReflectionType();

   // ----------------------------------------------------------------------
   // Instantiation
   // ----------------------------------------------------------------------

   /**
    * Sets up an instantiator of the represented type.
    */
   void setupInstantiator( SerializableTypeInstantiator* instantiator );

   /**
    * Creates an instance of this type.
    */
   template< typename T >
   T* instantiate() const;

   // ----------------------------------------------------------------------
   // Type definition
   // ----------------------------------------------------------------------

   /**
    * Defines a base type of this type ( in order to set up an inheritance hierarchy ).
    *
    * @param baseTypeName
    */
   void addBaseType( const std::string& baseTypeName );

   /**
    * Adds a new type member field.
    *
    * @param member     type member field description
    */
   void addMemberField( ReflectionTypeComponent* member );

   /**
    * Looks for a member with the specified member id.
    *
    * @param      memberId
    * @return     pointer to the member definition, or NULL if such a member is not defined
    */
   ReflectionTypeComponent* findMemberField( uint memberId ) const;

   /**
    * Looks for a member with the specified member name.
    *
    * @param      memberName
    * @return     pointer to the member definition, or NULL if such a member is not defined
    */
   ReflectionTypeComponent* findMemberField( const std::string& memberName ) const;

   /**
    * Creates the properties for this type and the specified object.
    *
    * @param hostObj
    * @param outProperties
    */
   void collectProperties( void* hostObj, std::vector< ReflectionProperty* >& outProperties ) const;

   /**
    * Tells if the type is abstract or not ( can it be instantiated or not ).
    */
   inline bool isAbstract() const;

   /**
    * Collects the parent types of this type.
    */
   void collectParents( std::vector< const SerializableReflectionType* >& outParentTypes ) const;

   /**
    * The opposite of the `collectParents` method - collects the types to which
    * this type is a parent.
    *
    * @param outDerivedTypes
    * @param includeAbstractTypes      should abstract implementations be taken into consideration?
    */
   void collectImplementations( std::vector< const SerializableReflectionType* >& outDerivedTypes, bool includeAbstractTypes ) const;

   /**
    * Returns a list of all types present in this type's hierarchy.
    */
   void mapTypesHierarchy( std::list< const SerializableReflectionType* >& outReflectionTypesList ) const;

   // ----------------------------------------------------------------------
   // Patching support
   // ----------------------------------------------------------------------

   /**
    * Sets up a patching information for a member field.
    *
    * CATION: the member types MUST match, otherwise the patching won't work
    * 
    * @param oldMemberName    how the member used to be named
    * @param newMemberName    what's the new member name
    */
   void patchMemberField( const std::string& oldMemberName, const std::string& newMemberName );

   // ----------------------------------------------------------------------
   // Serialization support
   // ----------------------------------------------------------------------

   /**
    * Serializes an object representation that corresponds to this type definition.
    *
    * @param object              serialized object
    * @param dependenciesMapper  dependencies (embedded objects) mapper
    * @param stream              output data stream to which the object should be serialized
    */
   template< typename T >
   void save( const T& object, const ReflectionSaver& dependenciesMapper, OutStream& stream ) const;

   /**
    * Deserializes an object representation that corresponds to this type definition.
    *
    * @param stream           input data stream that contains the serialized object
    */
   template< typename T >
   static T* load( InStream& stream );


   // ----------------------------------------------------------------------
   // ReflectionType implementation
   // ----------------------------------------------------------------------
   bool isA( const ReflectionType& referenceType ) const;

private:
   template< typename T >
   void saveMemberFields( const T* object, const ReflectionSaver& dependenciesMapper, OutStream& stream ) const;

   template< typename T >
   void loadMemberFields( T* object, InStream& stream ) const;

};

///////////////////////////////////////////////////////////////////////////////

/**
 * Returns an offset of the specified class member.
 */
#define MEMBER_OFFSET( OBJECT, MEMBER_NAME ) int( reinterpret_cast< long >( &(OBJECT)->MEMBER_NAME ) ) )

///////////////////////////////////////////////////////////////////////////////

#include "core/ReflectionType.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _REFLECTION_TYPE_H
