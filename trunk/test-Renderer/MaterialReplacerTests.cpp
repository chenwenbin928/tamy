#include "core-TestFramework\TestFramework.h"
#include "core-Renderer\MaterialReplacer.h"
#include "core-Renderer\Material.h"
#include "AbstractGraphicalNodeMock.h"
#include "LightReflectingPropertiesStub.h"
#include "MaterialOperationImplementationMock.h"
#include "TransparencyEnablerStub.h"


///////////////////////////////////////////////////////////////////////////////

TEST(MaterialReplacer, replacingSelectedMaterial)
{
   MaterialOperationImplementationMock matOp;
   TransparencyEnablerStub transparencyEnabler;

   Material material1("material1", new LightReflectingPropertiesStub(), matOp, matOp, transparencyEnabler);
   Material material2("material2", new LightReflectingPropertiesStub(), matOp, matOp, transparencyEnabler);

   AbstractGraphicalNodeMock* graphNode1 = new AbstractGraphicalNodeMock(material1);
   AbstractGraphicalNodeMock* graphNode2 = new AbstractGraphicalNodeMock(material2);
   AbstractGraphicalNodeMock* graphNode3 = new AbstractGraphicalNodeMock(material2);

   Node* compositeNode = new Node("compositeNode", false);
   compositeNode->addChild(graphNode1);
   compositeNode->addChild(graphNode2);

   Node rootNode("rootNode", true);
   rootNode.addChild(compositeNode);
   rootNode.addChild(graphNode3);
   
   // run the tests
   MaterialReplacer replacer(material2, material1);
   rootNode.accept(replacer);

   CPPUNIT_ASSERT_EQUAL(material1.getName(), graphNode1->getMaterial().getName());
   CPPUNIT_ASSERT_EQUAL(material1.getName(), graphNode2->getMaterial().getName());
   CPPUNIT_ASSERT_EQUAL(material1.getName(), graphNode3->getMaterial().getName());
}

///////////////////////////////////////////////////////////////////////////////