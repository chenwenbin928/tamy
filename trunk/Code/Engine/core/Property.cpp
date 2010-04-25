#include "core\Property.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_RTTI(Property)
END_RTTI

///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_RTTI(VectorProperty)
   PARENT(Property)
END_RTTI

///////////////////////////////////////////////////////////////////////////////

Property::Property()
: m_observer(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

void Property::setObserver(PropertyObserver& observer)
{
   m_observer = &observer;
}

///////////////////////////////////////////////////////////////////////////////

void Property::notifyAboutChange()
{
   if (m_observer)
   {
      m_observer->onPropertyChanged(*this);
   }
}

///////////////////////////////////////////////////////////////////////////////
