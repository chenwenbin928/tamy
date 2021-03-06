#ifndef _COMPONENTS_MANAGER_H
#error "This file can only be included from ComponentsManager.h"
#else

#include "core\SingletonsManager.h"
#include "core\Assert.h"
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
ComponentsManager< Derived >::ComponentsManager() 
: m_services( new SingletonsManager( ))
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
ComponentsManager< Derived >::~ComponentsManager() 
{
   removeAllComponents();

   delete m_services; m_services = NULL;
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::update( float timeElapsed )
{
   unsigned int count = m_comps.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      m_comps[i]->update( timeElapsed );
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::addComponent( Component< Derived >* component )
{
   Derived& derivedMgr = *( dynamic_cast< Derived* >( this ) );
   component->initialize( derivedMgr );
   component->onServiceRegistered( derivedMgr );
   
   m_comps.push_back( component );
   onComponentAdded( *component );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::removeComponent( Component< Derived >& component )
{
   for ( ComponentsArr::iterator it = m_comps.begin(); it != m_comps.end(); ++it )
   {
      if ( *it == &component )
      {
         onComponentRemoved( component );
         m_comps.erase( it );
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::removeAllComponents()
{
   ComponentsArr compsToRemove = m_comps;
   m_comps.clear();

   int count = (int)compsToRemove.size();
   for ( int i = count - 1; i >= 0; --i )
   {
      delete compsToRemove[i];
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
Component<Derived>* ComponentsManager< Derived >::getComponent( unsigned int idx )
{
   ASSERT_MSG( idx < m_comps.size(), "Component index out of range" );
   return m_comps[idx];
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::findComponents( const std::string& name, 
                                                   ComponentsArr& outComponents ) const
{
   unsigned int count = m_comps.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      if ( m_comps[ i ]->getComponentName() == name )
      {
         outComponents.push_back( m_comps[ i ] );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template< typename T >
void ComponentsManager< Derived >::findComponents( std::vector< T* >& outComponents ) const
{
   unsigned int count = m_comps.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      T* comp = dynamic_cast< T* >( m_comps[ i ] );
      if ( comp != NULL )
      {
         outComponents.push_back( comp );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template< typename T >
void ComponentsManager< Derived >::registerService( Component< Derived >& host, T& service )
{
   m_services->removeShared< T >();
   host.removeService( &service );

   bool result = m_services->setShared< T >( service );
   if ( result == false )
   {
      ASSERT_MSG( false, "Service is already registered" );
      return;
   }
   host.addService( &service );

   notifyAboutService( &service );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template< typename T >
void ComponentsManager< Derived >::removeService( Component< Derived >& host )
{
   if ( !m_services->hasShared< T >() )
   {
      return;
   }

   T& service = m_services->shared< T >();
   m_services->removeShared< T >();

   notifyAboutService( &service );
   host.removeService( &service );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template< typename T >
bool ComponentsManager< Derived >::hasService() const
{
   return m_services->hasShared< T >();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template < typename T >
bool ComponentsManager< Derived >::needsUpdate( T& service ) const
{
   return ( &service != NULL && m_services->hasShared< T >() == false )
      || ( m_services->hasShared< T >() && &m_services->shared< T >() != &service );
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
template< typename T >
T& ComponentsManager< Derived >::requestService()
{
   return m_services->shared< T >();
}

///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
unsigned int ComponentsManager< Derived >::getComponentsCount() const
{
   return m_comps.size();
}


///////////////////////////////////////////////////////////////////////////////

template< typename Derived >
void ComponentsManager< Derived >::notifyAboutService( void* service )
{
   Derived& derivedMgr = *( dynamic_cast< Derived* >( this ) );

   unsigned int count = m_comps.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      if ( m_comps[ i ]->isHost( service ) == false )
      {
         m_comps[ i ]->onServiceRegistered( derivedMgr );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _COMPONENTS_MANAGER_H
