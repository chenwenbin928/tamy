#ifndef _PROPERTY_H
#error "This file can only be included from Property.h"
#else


///////////////////////////////////////////////////////////////////////////////

void Property::setParams( const std::string& name, const std::string& label, bool canBeEdited, bool canBeSaved )
{
   m_name = name;
   m_label = label;
   m_canBeEdited = canBeEdited;
   m_canBeSaved = canBeSaved;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Property::getName() const
{
   return m_name;
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Property::getLabel() const
{
   return Property::m_label;
}

///////////////////////////////////////////////////////////////////////////////

bool Property::canBeEdited() const
{
   return m_canBeEdited;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TProperty< T >::TProperty() 
: m_val(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TProperty< T >::TProperty( T* val ) 
: m_val(val)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< T >::serialize(Serializer& serializer) 
{ 
   serializer << *m_val; 
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< T >::set( void* val )
{
   notifyBeforeChange();
   *m_val = *( reinterpret_cast< T* >( val ) );
   notifyAfterChange();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TProperty< T >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T >::getVirtualClass() const
{
   return Class( getClassesRegistry().getClassByType< T >() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T >::getPropertyClass() const
{
   return Class( getClassesRegistry().getClassByType< T >() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T >::getRTTIClass()
{
   return Class( getClassesRegistry().getClassByType< T >() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
TProperty<T*>::TProperty() 
: m_val(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TProperty< T* >::TProperty( T** val ) 
: m_val(val)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< T* >::serialize(Serializer& serializer) 
{ 
   Serializable* newValPtr = dynamic_cast<Serializable*> (*m_val);
   serializer << &newValPtr; 

   if ( newValPtr )
   {
      *m_val = reinterpret_cast<T*> (newValPtr);
   }
   else
   {
      *m_val = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< T* >::set( void* val )
{
   notifyBeforeChange();
   *m_val = *( reinterpret_cast< T** >( val ) );
   notifyAfterChange();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TProperty< T* >::edit()
{
   return m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T* >::getVirtualClass() const
{
   if ( *m_val )
   {
      return (*m_val)->getVirtualClass();
   }
   else
   {
      return Class( getClassesRegistry().getClassByType< T >() );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T* >::getPropertyClass() const
{
   return Class( getClassesRegistry().getClassByType< T >() );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TProperty< T* >::getRTTIClass()
{
   return Class( getClassesRegistry().getClassByType< T >() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TProperty< std::vector< T* > >::TProperty( std::vector< T* >* val ) 
: m_val(val)
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< std::vector< T* > >::serialize(Serializer& serializer) 
{ 
   serializer << *m_val;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< std::vector< T* > >::set( void* val )
{
   notifyBeforeChange();
   notifyAfterChange();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void* TProperty< std::vector< T* > >::edit()
{
   return this;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
unsigned int TProperty< std::vector< T* > >::size() const
{
   return m_val->size();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TProperty< std::vector< T* > >::viewProperties( unsigned int idx, PropertiesView& view )
{
   T* item = ( *m_val )[idx];
   if ( item )
   {
      item->viewProperties( view );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template< typename T >
TEditableProperty< T >::TEditableProperty( Property& property )
: m_property( property )
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
TEditableProperty< T >::~TEditableProperty()
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const std::string& TEditableProperty< T >::getLabel() const
{
   return m_property.getLabel();
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
void TEditableProperty< T >::set( const T& val ) 
{
   m_property.set( ( void* )( &val ) );
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
const T& TEditableProperty< T >::get() const 
{
   void* val = m_property.edit();
   ASSERT_MSG( val != NULL, "Non-pointer properties must be initialized before thay can be edited" );

   T* typedVal = reinterpret_cast< T* >( val );

   return *typedVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
T& TEditableProperty< T >::get() 
{
   void* val = m_property.edit();
   ASSERT_MSG( val != NULL, "Non-pointer properties must be initialized before thay can be edited" );

   T* typedVal = reinterpret_cast< T* >( val );

   return *typedVal;
}

///////////////////////////////////////////////////////////////////////////////

template< typename T >
Class TEditableProperty< T >::getType() const
{
   return m_property.getPropertyClass();
}

///////////////////////////////////////////////////////////////////////////////

#endif // _PROPERTY_H
