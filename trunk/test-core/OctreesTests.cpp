#include "core-TestFramework\TestFramework.h"
#include <d3dx9.h>
#include "core\Octree.h"
#include "core\BoundingSphere.h"


///////////////////////////////////////////////////////////////////////////////

class BoundedObjectMock
{
private:
   BoundingSphere m_boundingSphere;

public:
   BoundedObjectMock(float ox, float oy, float oz, float rad)
      : m_boundingSphere(D3DXVECTOR3(ox, oy, oz), rad)
   {}

   const BoundingSphere& getBoundingVolume() const {return m_boundingSphere;}
};


///////////////////////////////////////////////////////////////////////////////

TEST(Octree, queryingElements)
{
   Octree<BoundedObjectMock> tree;
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(0, 0, 0, 1);

   tree.insert(ob1);

   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);

   // the element is also fetched from a query that doesn't necessarily
   // collide with it, but instead collides with a sector it's in
   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(10, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());
}

///////////////////////////////////////////////////////////////////////////////

TEST(Octree, sectorsDivision)
{
   Octree<BoundedObjectMock> tree(1, 20);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(-5, 5, 5, 1);
   BoundedObjectMock ob2(5, 5, 5, 1);
   BoundedObjectMock ob3(5, -5, 5, 2);

   tree.insert(ob1);
   tree.insert(ob2);
   tree.insert(ob3);


   tree.query(BoundingSphere(D3DXVECTOR3(-5, 5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(5, 5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(5, -5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob3, result[0]);

   // this query should return the data from all the overlapping sectors
   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 20), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob2, result[1]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[2]);
}

///////////////////////////////////////////////////////////////////////////////

TEST(Octree, elementsThatSpanAcrossSectors)
{
   Octree<BoundedObjectMock> tree(1, 20);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock spanningOb(0, 0, 0, 2);
   BoundedObjectMock smallOb(1, 1, 1, 1);

   tree.insert(spanningOb);
   tree.insert(smallOb);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(1, 1, 1), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&spanningOb, result[0]);
   CPPUNIT_ASSERT_EQUAL(&smallOb, result[1]);


   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&spanningOb, result[0]);
   CPPUNIT_ASSERT_EQUAL(&smallOb, result[1]);
}

///////////////////////////////////////////////////////////////////////////////

TEST(Octree, removingElements)
{
   Octree<BoundedObjectMock> tree(1, 20);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(-1, 1, 1, 1);
   BoundedObjectMock ob2(0, 0, 0, 2);
   BoundedObjectMock ob3(1, 1, 1, 1);

   tree.insert(ob1);
   tree.insert(ob2);
   tree.insert(ob3);

   // initially all the objects are there
   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob2, result[1]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[2]);

   // removing first object
   tree.remove(ob1);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[1]);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(-1, 1, 1), 0.5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);

   // removing second object
   tree.remove(ob2);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob3, result[0]);

   // removing the last object
   tree.remove(ob3);

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());

   result.clear();
   tree.query(BoundingSphere(D3DXVECTOR3(1, 1, 1), 0.5f), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());
}

///////////////////////////////////////////////////////////////////////////////
