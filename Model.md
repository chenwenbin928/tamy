# Model #

## Prefabs ##

Prefabs are models we can nest in other models.
This helps us build up our scene from well defined, reusable building blocks.

There is no specific class for a `Prefab` - you simply create a `Model` with the entities you want and embed it in another model using `ModelEntity`.

Each entity in a model will be cloned to provide the new model with an instanced copy of self.
The trick with this approach is to provide a mechanism taht will ensure that if we cahnge a property of an entity, all of its clones will see that change and reflect it.

### Value sharing ###

There's a special, generic wrapper class - `TShared`. Whatever value is wrapped in it will be shared when we copy the value from one wrapper to the next:

```
   TShared< int > valueA = 5;
   TShared< int > valueB = valueA;

   CPPUNIT_ASSERT_EQUAL( 5, valueA.get() );
   CPPUNIT_ASSERT_EQUAL( 5, valueB.get() );

   valueB = 1;

   CPPUNIT_ASSERT_EQUAL( 1, valueA.get() );
   CPPUNIT_ASSERT_EQUAL( 1, valueB.get() );
```

Use this wrapper for entity proeprties you want shared among all entity clones.