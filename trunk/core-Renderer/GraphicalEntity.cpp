#include "GraphicalEntity.h"
#include "Material.h"
#include "GraphicalNode.h"
#include "GraphicalEntityInstantiator.h"
#include <sstream>


///////////////////////////////////////////////////////////////////////////////

GraphicalEntity::GraphicalEntity(const std::string& name,
                                 const std::vector<Material*>& materials)
      : LeafGraphicalEntity(name),
      m_materials(materials)
{
}

///////////////////////////////////////////////////////////////////////////////

GraphicalEntity::~GraphicalEntity()
{
}

///////////////////////////////////////////////////////////////////////////////

Material& GraphicalEntity::getMaterial(DWORD subsetIdx) const 
{
   if (subsetIdx >= m_materials.size())
   {
      throw std::out_of_range("There aren't so many subsets defined");
   }
   return *(m_materials.at(subsetIdx));
}

///////////////////////////////////////////////////////////////////////////////

unsigned int GraphicalEntity::getNumSubsets() const
{
   return m_materials.size();
}

///////////////////////////////////////////////////////////////////////////////

Node* GraphicalEntity::instantiate()
{
   Node* rootNode = new Node(getName());
   rootNode->setLocalMtx(getLocalMtx());

   unsigned int subsetsCount = m_materials.size();
   for (unsigned int subsetIdx = 0; subsetIdx < subsetsCount; ++subsetIdx)
   {
      std::stringstream childNodeName;
      childNodeName << getName() << "_subset" << subsetIdx;
      Node* newNode = new GraphicalNode(childNodeName.str(), *this, subsetIdx);
      rootNode->addChild(newNode);
   }

   return rootNode;
}

///////////////////////////////////////////////////////////////////////////////

void GraphicalEntity::accept(GraphicalEntityInstantiator& instantiator)
{
   instantiator.visit(*this);
}

///////////////////////////////////////////////////////////////////////////////
