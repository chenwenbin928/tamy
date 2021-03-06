#include "core.h"
#include "core\ReflectionProperty.h"
#include "core\ReflectionTypesRegistry.h"
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

ReflectionProperty::ReflectionProperty( ReflectionObject* hostObject )
: m_observer( hostObject )
, m_name( "" )
, m_label( "" )
, m_canBeEdited( false )
, m_canBeSaved( true )
{
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionProperty::notifyBeforeChange()
{
   if ( m_observer )
   {
      m_observer->onPrePropertyChanged( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ReflectionProperty::notifyAfterChange()
{
   if ( m_observer )
   {
      m_observer->onPropertyChanged( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ReflectionPropertyArray::ReflectionPropertyArray( ReflectionObject* hostObject )
   : ReflectionProperty( hostObject )
{
}

///////////////////////////////////////////////////////////////////////////////
