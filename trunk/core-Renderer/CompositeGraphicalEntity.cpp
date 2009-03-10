#include "CompositeGraphicalEntity.h"
#include "GraphicalEntityInstantiator.h"
#include "Node.h"


///////////////////////////////////////////////////////////////////////////////

CompositeGraphicalEntity::CompositeGraphicalEntity(const std::string& name,
                                                   const D3DXMATRIX& localMtx)
      : AbstractGraphicalEntity(name),
      m_localMtx(localMtx)
{
}

///////////////////////////////////////////////////////////////////////////////

CompositeGraphicalEntity::~CompositeGraphicalEntity()
{
   for (std::list<AbstractGraphicalEntity*>::iterator it = m_children.begin();
        it != m_children.end(); ++it)
   {
      delete *it;
   }
   m_children.clear();
}

///////////////////////////////////////////////////////////////////////////////

void CompositeGraphicalEntity::addChild(AbstractGraphicalEntity* child)
{
   m_children.push_back(child);
}

///////////////////////////////////////////////////////////////////////////////

const std::list<AbstractGraphicalEntity*>& CompositeGraphicalEntity::getChildren() const
{
   return m_children;
}

///////////////////////////////////////////////////////////////////////////////

Node* CompositeGraphicalEntity::instantiate()
{
   Node* subtreeRoot = new Node(getName());
   subtreeRoot->setLocalMtx(getLocalMtx());

   return subtreeRoot;
}

///////////////////////////////////////////////////////////////////////////////

void CompositeGraphicalEntity::accept(GraphicalEntityInstantiator& instantiator)
{
   instantiator.visit(*this);
}

///////////////////////////////////////////////////////////////////////////////