#pragma once



///////////////////////////////////////////////////////////////////////////////

class AbstractGraphicalNode;
struct BoundingSphere;

///////////////////////////////////////////////////////////////////////////////

class AGNVolExtractor
{
public:
   const BoundingSphere& operator()(AbstractGraphicalNode* elem) const;
};

///////////////////////////////////////////////////////////////////////////////
