#include "SkinnedGraphicalNode.h"
#include "SkinnedGraphicalEntity.h"


///////////////////////////////////////////////////////////////////////////////

SkinnedGraphicalNode::SkinnedGraphicalNode(const std::string& name,
                                           SkinnedGraphicalEntity& entity,
                                           DWORD subset,
                                           const std::vector<std::pair<Node*, D3DXMATRIX> >& bones) 
      : AbstractGraphicalNode(name, entity.getMaterial(subset), subset), 
      m_entity(entity),
      m_bones(bones)
{
}

///////////////////////////////////////////////////////////////////////////////

void SkinnedGraphicalNode::render()
{
   std::vector<D3DXMATRIX> matrices(m_bones.size());
   for (unsigned int i = 0; i < m_bones.size(); ++i)
   {
      const D3DXMATRIX& offsetMtx = m_bones.at(i).second;
      const D3DXMATRIX& globalMtx = m_bones.at(i).first->getGlobalMtx();
      D3DXMatrixMultiply(&(matrices[i]), &offsetMtx, &globalMtx);
   }

   m_entity.render(matrices, m_subset);
}

///////////////////////////////////////////////////////////////////////////////
