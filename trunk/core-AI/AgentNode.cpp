#include "core-AI\AgentNode.h"
#include "core\NodeVisitor.h"
#include "core\TNodesVisitor.h"


///////////////////////////////////////////////////////////////////////////////

void AgentNode::onAccept(NodeVisitor& visitor)
{
   REGISTER_NODE_VISITOR(TNodesVisitor<AgentNode>);
}

///////////////////////////////////////////////////////////////////////////////
