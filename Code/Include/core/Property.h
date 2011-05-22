#ifndef _PROPERTY_H
#define _PROPERTY_H

/// @file   core\Property.h
/// @brief  generic property interface

#include <string>
#include <map>
#include <vector>
#include "core\Serializer.h"
#include "core\Class.h"


///////////////////////////////////////////////////////////////////////////////

class Object;
class Property;
class PropertiesView;

///////////////////////////////////////////////////////////////////////////////

class PropertyObserver
{
public:
   virtual ~PropertyObserver() {}

   /**
    * Called when a property value gets changed.
    */
   virtual void onPropertyChanged(Property& property) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This is a generic property interface. It will allow to store properties
 * of various data types in a uniform manner in all sorts of collections.
 *
 * A property can be observed - its observers will get notified about changes 
 * the property undergoes.
 */
class Property
{
private:
   PropertyObserver* m_observer;
   std::string       m_name;
   std::string       m_label;
   bool              m_canBeEdited;
   bool              m_canBeSaved;

public:
   virtual ~Property() {}

   /**
    * Serializes the property value.
    *
    * @param   serializer
    */
   virtual void serialize(Serializer& serializer) = 0;

   /**
    * Sets the property name.
    *
    * @param name
    * @param label
    * @param canBeEdited
    * @param canBeSaved
    */
   inline void setParams( const std::string& name, const std::string& label, bool canBeEdited, bool canBeSaved );

   /**
    * This method returns the name assigned to this property.
    *
    * @return  property name
    */
   inline const std::string& getName() const;

   /**
    * This method returns a label under which the property should
    * be advertised (in editor i.e.)
    *
    * @return  property label
    */
   inline const std::string& getLabel() const;

   /**
    * Tells whether we want to edit this property or not.
    */
   inline bool canBeEdited() const;

   /**
    * Sets a new value of the property.
    *
    * @param val
    */
   virtual void set( void* val ) = 0;

   /**
    * Allows to edit the property contents by a property editor.
    */
   virtual void* edit() = 0;

   // -------------------------------------------------------------------------
   // Type identification mechanism.
   // -------------------------------------------------------------------------
   /**
    * Retrieves the virtual (topmost in terms of inheritance hierarchy) class
    * of the element stored in the property.
    *
    * i.e. lets take the following classes hierarchy:
    *       class A {};  class B : public A {};
    *    and property setup:
    *       B* val;
    *       TProperty< A* > property( val );
    *    then:
    *       property.getVirtualClass() == B;
    */
   virtual Class getVirtualClass() const = 0;

   /**
    * Returns the type the property is parametrized with.
    */
   virtual Class getPropertyClass() const = 0;

   // -------------------------------------------------------------------------
   // Observation mechanism
   // -------------------------------------------------------------------------
   /**
    * Retrieves the observer of this property.
    */
   PropertyObserver& getObserver();

   /**
    * Sets a new property observer.
    * 
    * @param observer
    */
   void setObserver(PropertyObserver& observer);

   /**
    * Call this method when the value of the property changes.
    */
   void notifyAboutChange();

protected:
   /**
    * Constructor.
    */
   Property();

};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property allows to create properties for all types of data.
 */
template <typename T>
class TProperty : public Property
{
private:
   T*             m_val;

public:
   TProperty();
   TProperty( T* val );

   // -------------------------------------------------------------------------
   // Property implementation
   // -------------------------------------------------------------------------
   void serialize(Serializer& serializer);
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   Class getVirtualClass() const;
   Class getPropertyClass() const;
   static Class getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property allows to create properties for all types of pointers.
 */
template <typename T>
class TProperty<T*> : public Property
{
private:
   T** m_val;

public:
   TProperty();
   TProperty(T** val );

   // -------------------------------------------------------------------------
   // Property implementation
   // -------------------------------------------------------------------------
   void serialize(Serializer& serializer);
   void set( void* val );
   void* edit();

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   Class getVirtualClass() const;
   Class getPropertyClass() const;
   static Class getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A marker interface for the properties that hold STL vectors.
 * It's needed so that we can write generic editors for those properties.
 */
class VectorProperty : public Property
{
private:
   class RTTITypeInit
   {
   public:
      RTTITypeInit();
      void realize() {};
   };

private:
   static Class         s_class;
   static RTTITypeInit  s_typeRegistrationTool;

   std::string          m_emptyStr;

public:
   virtual ~VectorProperty() {}

   /**
    * Returns the size of the array.
    */
   virtual unsigned int size() const { return 0; }

   /**
    * Allows to view the properties of the selected item.
    *
    * @param idx     item's index
    * @param view    properties viewer
    */
   virtual void viewProperties( unsigned int idx, PropertiesView& view ) {}

   // -------------------------------------------------------------------------
   // Property implementation
   // -------------------------------------------------------------------------
   const std::string& getName() const { return m_emptyStr; }
   const std::string& getLabel() const { return m_emptyStr; }
   void serialize( Serializer& serializer ) {}
   void set( void* val ) {}
   void* edit() { return NULL; }

   // -------------------------------------------------------------------------
   // Type identification mechanism implementation.
   // -------------------------------------------------------------------------
   Class getVirtualClass() const;
   Class getPropertyClass() const;
   static Class getRTTIClass();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This specialized property can hold a vector of pointers
 */
template< typename T >
class TProperty< std::vector< T* > > : public VectorProperty
{
private:
   std::vector<T*>*     m_val;

public:
   TProperty( std::vector<T*>* val );

   // -------------------------------------------------------------------------
   // VectorProperty implementation
   // -------------------------------------------------------------------------
   unsigned int size() const;
   void viewProperties( unsigned int idx, PropertiesView& view );

   // -------------------------------------------------------------------------
   // Property implementation
   // -------------------------------------------------------------------------
   void serialize( Serializer& serializer );
   void set( void* val );
   void* edit();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Editable version of a property.
 */
template< typename T >
class TEditableProperty
{
   Property&   m_property;

public:
   TEditableProperty( Property& property );
   ~TEditableProperty();

   /**
    * This method returns a label under which the property should
    * be advertised (in editor i.e.)
    *
    * @return  property label
    */
   const std::string& getLabel() const;

   /**
    * Sets new property value.
    *
    * @param val     new property value
    */
   void set( const T& val );

   /**
    * Returns current property value (const version).
    *
    * @return        property value.
    */
   const T& get() const;

   /**
    * Returns current property value.
    *
    * @return        property value.
    */
   T& get();

   /**
    * Returns the type the property is parametrized with.
    */
   Class getType() const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core\Property.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _PROPERTY_H