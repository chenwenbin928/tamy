# Scene Render Widget #

## Debug renderer ##

We have a dedicated renderer that deals in visual debug representations of our scene objects - `EditorDebugRenderer`.

It's got 2 methods that allow you to add or remove debug entities:

```
   /**
    * Adds a new entity for debug rendering.
    *
    * @param entity
    */
   void add( Entity& entity );

   /**
    * Adds a new entity for debug rendering.
    *
    * @param entity
    */
   void remove( Entity& entity );
```


However - in order to keep constant track of the entities from the main scene, another manager was created - `DebugEntitiesManager`.
It's a `ModelView` that's attached to the main scene and keeps track of the entities populating it, creating debug representations for them as it goes along.

## Creating a debug representation of a scene entity ##

If you want to create a new representation for your engine or game entity, simply derive from `DebugGeometry` and put your new class along with other representations in this filter -> `TamyEditor/Widgets/SceneRenderWidget/DebugRepresentations`.

Next - register it with `DebugEntitiesManager` in its constructor, and that's it.