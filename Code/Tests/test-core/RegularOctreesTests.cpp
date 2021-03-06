#include "core-TestFramework\TestFramework.h"
#include <d3dx9.h>
#include "core\RegularOctree.h"
#include "core\BoundingSphere.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class BoundedObjectMock
   {
   private:
      BoundingSphere m_boundingSphere;

   public:
      BoundedObjectMock(float ox, float oy, float oz, float rad)
         : m_boundingSphere(Vector(ox, oy, oz), rad)
      {}

      const BoundingSphere& getBoundingVolume() const {return m_boundingSphere;}
   };

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

TEST(RegularOctree, queryingElements)
{
   AABoundingBox treeBB(Vector(-10, -10, -10), Vector(10, 10, 10));
   RegularOctree<BoundedObjectMock> tree(treeBB);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(0, 0, 0, 1);

   tree.insert(ob1);

   tree.query(BoundingSphere(Vector(0, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);

   // the element is also fetched from a query that doesn't necessarily
   // collide with it, but instead collides with a sector it's in
   result.clear();
   tree.query(BoundingSphere(Vector(10, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());
}

///////////////////////////////////////////////////////////////////////////////

TEST(RegularOctree, sectorsDivision)
{
   AABoundingBox treeBB(Vector(-10, -10, -10), Vector(10, 10, 10));
   RegularOctree<BoundedObjectMock> tree(treeBB, 1);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(-5, 5, 5, 1);
   BoundedObjectMock ob2(5, 5, 5, 1);
   BoundedObjectMock ob3(5, -5, 5, 2);

   tree.insert(ob1);
   tree.insert(ob2);
   tree.insert(ob3);


   tree.query(BoundingSphere(Vector(-5, 5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);

   result.clear();
   tree.query(BoundingSphere(Vector(5, 5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);

   result.clear();
   tree.query(BoundingSphere(Vector(5, -5, 5), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob3, result[0]);

   // this query should return the data from all the overlapping sectors
   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 20), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob2, result[1]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[2]);
}

///////////////////////////////////////////////////////////////////////////////

TEST(RegularOctree, elementsThatSpanAcrossSectors)
{
   AABoundingBox treeBB(Vector(-10, -10, -10), Vector(10, 10, 10));
   RegularOctree<BoundedObjectMock> tree(treeBB, 1);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock spanningOb(0, 0, 0, 2);
   BoundedObjectMock smallOb(1, 1, 1, 1);

   tree.insert(spanningOb);
   tree.insert(smallOb);

   result.clear();
   tree.query(BoundingSphere(Vector(1, 1, 1), 1), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&spanningOb, result[0]);
   CPPUNIT_ASSERT_EQUAL(&smallOb, result[1]);


   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&spanningOb, result[0]);
   CPPUNIT_ASSERT_EQUAL(&smallOb, result[1]);
}

///////////////////////////////////////////////////////////////////////////////

TEST(RegularOctree, removingElements)
{
   AABoundingBox treeBB(Vector(-10, -10, -10), Vector(10, 10, 10));
   RegularOctree<BoundedObjectMock> tree(treeBB, 1);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(-1, 1, 1, 1);
   BoundedObjectMock ob2(0, 0, 0, 2);
   BoundedObjectMock ob3(1, 1, 1, 1);

   tree.insert(ob1);
   tree.insert(ob2);
   tree.insert(ob3);

   // initially all the objects are there
   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)3, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob1, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob2, result[1]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[2]);

   // removing first object
   tree.remove(ob1);

   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)2, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);
   CPPUNIT_ASSERT_EQUAL(&ob3, result[1]);

   result.clear();
   tree.query(BoundingSphere(Vector(-1, 1, 1), 0.5), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob2, result[0]);

   // removing second object
   tree.remove(ob2);

   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)1, result.size());
   CPPUNIT_ASSERT_EQUAL(&ob3, result[0]);

   // removing the last object
   tree.remove(ob3);

   result.clear();
   tree.query(BoundingSphere(Vector(0, 0, 0), 2), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());

   result.clear();
   tree.query(BoundingSphere(Vector(1, 1, 1), 0.5f), result);
   CPPUNIT_ASSERT_EQUAL((unsigned int)0, result.size());
}

///////////////////////////////////////////////////////////////////////////////

TEST(RegularOctree, sceneBoundsCalculation)
{
   AABoundingBox treeBB(Vector(-10, -10, -10), Vector(10, 10, 10));
   RegularOctree<BoundedObjectMock> tree(treeBB, 1);
   Array<BoundedObjectMock*> result;

   BoundedObjectMock ob1(0, 0, 0, 2);
   BoundedObjectMock ob2(3, 3, 3, 2);

   AABoundingBox actualSceneBounds;

   tree.insert(ob1);
   tree.getSceneBounds( actualSceneBounds );
   COMPARE_VEC( Vector( -2, -2, -2 ), actualSceneBounds.min );
   COMPARE_VEC( Vector( 2, 2, 2 ), actualSceneBounds.max );

   tree.insert(ob2);
   tree.getSceneBounds( actualSceneBounds );
   COMPARE_VEC( Vector( -2, -2, -2 ), actualSceneBounds.min );
   COMPARE_VEC( Vector( 5, 5, 5 ), actualSceneBounds.max );

   tree.remove(ob1);
   tree.getSceneBounds( actualSceneBounds );
   COMPARE_VEC( Vector( 1, 1, 1 ), actualSceneBounds.min );
   COMPARE_VEC( Vector( 5, 5, 5 ), actualSceneBounds.max );
}

///////////////////////////////////////////////////////////////////////////////
