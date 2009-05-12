#pragma once

#include <set>
#include <d3dx9.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class AbstractGraphicalNode;
class Material;

///////////////////////////////////////////////////////////////////////////////

class BatchComparator
{
public:
   bool operator()(AbstractGraphicalNode* lhs, AbstractGraphicalNode* rhs) const;
};

///////////////////////////////////////////////////////////////////////////////