#include "AbstractGraphicalEntity.h"


///////////////////////////////////////////////////////////////////////////////

AbstractGraphicalEntity::AbstractGraphicalEntity(const std::string& name)
      : m_name(name)
{
}

///////////////////////////////////////////////////////////////////////////////

AbstractGraphicalEntity::~AbstractGraphicalEntity()
{
}

///////////////////////////////////////////////////////////////////////////////

void AbstractGraphicalEntity::setAnimationDefinition(const AnimationDefinition& animationDef)
{
   m_animationData = animationDef;
}

///////////////////////////////////////////////////////////////////////////////

Skeleton* AbstractGraphicalEntity::instantiateSkeleton(Node& rootBone)
{
   return new Skeleton(m_animationData, rootBone);
}

///////////////////////////////////////////////////////////////////////////////
