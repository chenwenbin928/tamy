# Reflection objects editors #

This feature allows you to create an object edition framework in your editor.

Just create your custom `ReflectionPropertiesView` instance
and call its `set` method, passing the object the properties of which you want to edit, and the framework will take care of instantiating
your custom editors for you - once you create the custom editors of course and registered them with that view.

Let's take a look how it's done.

## Custom editor ##
Let's start from the leaves of our system - that is from the custom editors dedicated to particular engine types.

Let's say we want to be able to edit an integer value our objects might hold as properties.

```

   class IntPropertyEditor : public ReflectionPropertyEditor
   {
   private:
      TEditableReflectionProperty< int >*         m_property;

   public:
      IntPropertyEditor( TEditableReflectionProperty< int >* property ) 
         : m_property( property )
      {
      }

      ~IntPropertyEditor()
      {
         delete m_property;
      }

      void initialize( ReflectionObjectEditor* parentEditor ) {}

      void deinitialize( ReflectionObjectEditor* parentEditor ) {}
   };
```

All you need to do is implement the `ReflectionPropertyEditor` interface.
`ReflectionPropertiesView` that's gonna be instantiating this class requires that you define a constructor that accepts
a single parameter of type `TEditableReflectionProperty< EditedType >`, where `EditedType` stands for the property type
this editor deals with ( `int` in this case ).

## Custom object view implementation ##

Here's how the most basic ( but fully operational ) implementation looks like.

```
class MyPropertiesView : public ReflectionPropertiesView
{
public:
   MyPropertiesView()
   {
	  // definitions
	  defineObjectNode< ObjectNodeEditor >();
	  
	  // associations
      associate< int, IntPropertyEditor >();
   }
};
```

It simply sets up an abstract factory, telling what editors should be created in response to what data types.

There's one definition that you absolutely NEED TO register in order for the view to be operational:
  * `defineObjectNode` - it defines the placeholder editor that will be used as a container for property editors crated for the edited object.

> There's also a couple of extra definitions you can provide, if you want to deal with certain types of data:
  * `defineEnums` - defines a common editor for all enum properties
  * `defineArray` - defines a common editor for all arrays of ReflectionObject-derrived pointers (i.e. std::vector< Resource**> or std::vector< ReflectionObject** > ).

You have a few association methods at your disposal that deal in different data types:
  * `associate` - for primitive types ( i.e: int, Vector, std::string  - anything that doesn't inherit from ReflectionObject )
  * `associateAbstract` - for objects that inherit from ReflectionObject. There's a trick here however - imagine you have a `class A` and `class B` derrived from it, and a custom editor for each of them. In order for those editors to be created correctly, you need to register the editor of the more specific type ( `class B` ) **first**, and the more generic one ( `class A` ) next.

## Architecture ##

`ReflectionPropertiesView` creates a tree composed of `ReflectionObjectEditor` instances. You can find the algorithm in `ReflectionPropertiesView::set` method.
Each `ReflectionObjectEditor` in turn holds a list of `ReflectionPropertyEditor` instances, corresponding to particular properties of the object.

If an array property is encountered ( and providing there's an editor defined for it ), an instance of `ReflectionPropertyEditorComposite` will be created. This class holds a list of `ReflectionObjectEditor` instances, thus creating a new branch of the tree.

Each `ReflectionObjectEditor` instance holds a reference to the edited object.