#pragma once

#include <set>


///////////////////////////////////////////////////////////////////////////////

class AbstractGraphicalNode;

///////////////////////////////////////////////////////////////////////////////

class BatchComparator
{
public:
   bool operator()(const AbstractGraphicalNode* lhs, const AbstractGraphicalNode* rhs) const;
};

///////////////////////////////////////////////////////////////////////////////

typedef std::multiset<AbstractGraphicalNode*, BatchComparator> BatchedNodes;

///////////////////////////////////////////////////////////////////////////////
