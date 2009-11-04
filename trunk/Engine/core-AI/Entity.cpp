#include "core-AI\Entity.h"


///////////////////////////////////////////////////////////////////////////////

unsigned long Entity::m_nextId = 0;

///////////////////////////////////////////////////////////////////////////////

Entity::Entity()
: m_id(m_nextId)
, m_situation(0, 0, 0)
{
   ++m_nextId;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long Entity::getID() const
{
   return m_id;
}

///////////////////////////////////////////////////////////////////////////////
