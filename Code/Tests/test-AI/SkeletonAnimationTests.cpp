#include "core-TestFramework/TestFramework.h"
#include "core-MVC/Entity.h"
#include "core-MVC/SpatialEntity.h"
#include "core-AI/SkeletonAnimationController.h"
#include "core-AI/SkeletonAnimation.h"
#include "core-AI/BoneSRTAnimation.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{

   class MockNonAnimatedEntity : public Entity
   {
      DECLARE_CLASS()
   public:
      MockNonAnimatedEntity( const std::string& id = "" ) : Entity( id ) {}

   protected:
      Entity* cloneSelf() const { return new MockNonAnimatedEntity(); }
   };
   BEGIN_OBJECT( MockNonAnimatedEntity );
      PARENT( Entity );
   END_OBJECT();

} // namespace anonymous

///////////////////////////////////////////////////////////////////////////////

DEFINE_TYPE_ID( MockNonAnimatedEntity );

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonAnimationController, architecture )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = ReflectionTypesRegistry::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< SpatialEntity >( "SpatialEntity", new TSerializableTypeInstantiator< SpatialEntity >() ); 
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() ); 
   typesRegistry.addSerializableType< MockNonAnimatedEntity >( "MockNonAnimatedEntity", new TSerializableTypeInstantiator< MockNonAnimatedEntity >() ); 
   typesRegistry.addSerializableType< SkeletonAnimation >( "SkeletonAnimation", new TSerializableTypeInstantiator< SkeletonAnimation >() ); 
   typesRegistry.addSerializableType< BoneSRTAnimation >( "BoneSRTAnimation", new TSerializableTypeInstantiator< BoneSRTAnimation >() ); 
   typesRegistry.addSerializableType< SkeletonAnimationController >( "SkeletonAnimationController", new TSerializableTypeInstantiator< SkeletonAnimationController >() ); 

   // create a hierarchy of nodes
   SpatialEntity* root = new SpatialEntity( "root" );
   SpatialEntity* hip = new SpatialEntity( "hip" );
   SpatialEntity* leg = new SpatialEntity( "leg" );
   SpatialEntity* anotherLeg = new SpatialEntity( "leg" ); // this one won't get updated due to a duplicated name
   MockNonAnimatedEntity* notANode = new MockNonAnimatedEntity( "notANode" );
   SpatialEntity* gun = new SpatialEntity( "gun" ); // this one won't get updated, because as you'll see in a few lines - it's connected
                                                    // via a non-spatial entity
   root->add( hip );
   root->add( notANode );
   hip->add( leg );
   hip->add( anotherLeg );
   notANode->add( gun );

   // provide an animation source
   SkeletonAnimation animSource;

   BoneSRTAnimation* rootBoneKeys = new BoneSRTAnimation( "root" );
   rootBoneKeys->addTranslationKey( 0.f, Quad_1000 );
   rootBoneKeys->addOrientationKey( 0.f, Quaternion::getIdentity() );
   animSource.addKeys( rootBoneKeys );

   BoneSRTAnimation* hipBoneKeys = new BoneSRTAnimation( "hip" );
   hipBoneKeys->addTranslationKey( 0.f, Quad_1000 );
   hipBoneKeys->addOrientationKey( 0.f, Quaternion::getIdentity() );
   animSource.addKeys( hipBoneKeys );

   BoneSRTAnimation* legBoneKeys = new BoneSRTAnimation( "leg" );
   legBoneKeys->addTranslationKey( 0.f, Quad_1000 );
   legBoneKeys->addOrientationKey( 0.f, Quaternion::getIdentity() );
   animSource.addKeys( legBoneKeys );

   // add an animation controller to the hierarchy
   SkeletonAnimationController* animController = new SkeletonAnimationController();
   animController->setAnimationSource( animSource );
   root->add( animController );

   // run the controller and verify the controller requests the animations for all the connected
   // spatial entities - but not the ones that are connected via non-spatial entities
   animController->update( 0.f );
   Matrix result, expected;
   Vector expectedTransl;
   expectedTransl.set( 1, 0, 0 );
   expected.setTranslation( expectedTransl );
   result = root->getGlobalMtx();
   COMPARE_MTX( expected, result );

   expectedTransl.set( 2, 0, 0 );
   expected.setTranslation( expectedTransl );
   result = hip->getGlobalMtx();
   COMPARE_MTX( expected, result );

   expectedTransl.set( 3, 0, 0 );
   expected.setTranslation( expectedTransl );
   result = leg->getGlobalMtx();
   COMPARE_MTX( expected, result );

   expectedTransl.set( 2, 0, 0 );
   expected.setTranslation( expectedTransl );
   result = anotherLeg->getGlobalMtx();
   COMPARE_MTX( expected, result );

   // this one is connected via a non-node entity - so it's considered to be detached from the skeleton
   expectedTransl.set( 0, 0, 0 );
   expected.setTranslation( expectedTransl );
   result = gun->getGlobalMtx();
   COMPARE_MTX( expected, result );

   // cleanup
   delete root;
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////

TEST( SkeletonAnimationController, trackTimeControl )
{
   // setup reflection types
   ReflectionTypesRegistry& typesRegistry = ReflectionTypesRegistry::getInstance();
   typesRegistry.clear();
   typesRegistry.addSerializableType< SpatialEntity >( "SpatialEntity", new TSerializableTypeInstantiator< SpatialEntity >() ); 
   typesRegistry.addSerializableType< Entity >( "Entity", new TSerializableTypeInstantiator< Entity >() ); 
   typesRegistry.addSerializableType< MockNonAnimatedEntity >( "MockNonAnimatedEntity", new TSerializableTypeInstantiator< MockNonAnimatedEntity >() ); 
   typesRegistry.addSerializableType< SkeletonAnimation >( "SkeletonAnimation", new TSerializableTypeInstantiator< SkeletonAnimation >() ); 
   typesRegistry.addSerializableType< BoneSRTAnimation >( "BoneSRTAnimation", new TSerializableTypeInstantiator< BoneSRTAnimation >() ); 
   typesRegistry.addSerializableType< SkeletonAnimationController >( "SkeletonAnimationController", new TSerializableTypeInstantiator< SkeletonAnimationController >() ); 

   // create the rig
   SpatialEntity* root = new SpatialEntity( "root" );
   SkeletonAnimationController* animController = new SkeletonAnimationController();
   root->add( animController );

   SkeletonAnimation animSource;

   BoneSRTAnimation* rootBoneKeys = new BoneSRTAnimation( "root" );
   rootBoneKeys->addTranslationKey( 3.f, Quad_0 );
   animSource.addKeys( rootBoneKeys );

   animController->setAnimationSource( animSource );

   // run the animation updates
   animController->update( 1.f );
   CPPUNIT_ASSERT_EQUAL( 1.f, animController->getTrackTime() );

   animController->update( 1.5f );
   CPPUNIT_ASSERT_EQUAL( 2.5f, animController->getTrackTime() );

   // the track time depends on the longest defined animation - so the next update will wrap the time around
   animController->update( 1.0f );
   CPPUNIT_ASSERT_EQUAL( 0.5f, animController->getTrackTime() );

   // .. the same applied for updates that last longer then the animation time
   animController->update( 7.0f );
   CPPUNIT_ASSERT_EQUAL( 1.5f, animController->getTrackTime() );

   // cleanup
   delete root;
   typesRegistry.clear();
}

///////////////////////////////////////////////////////////////////////////////
