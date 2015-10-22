# Multithreaded system design & usage #

## Purpose of main thread ##
## Where should I update my updatable class ##

The question you should ask yourself is - is this an object that's supposed to exist on the main thread, or on a worker thread?

If it's the former, make the object implement the `TimeDependent` mechanism and add it to a singleton instance of TimeController, as so:

```
TimeController& tc = TSingleton< TimeController >::getInstance();
tc.add( "MyObjectId", objectInstance );
```

If it's an asynchronous execution that you want, simply make it implement the `Runnable` interface and schedule it on a singleton instance of `MultithreadedTasksScheduler`:

```
MultithreadedTasksScheduler& ts = TSingleton< MultithreadedTasksScheduler>::getInstance();
ts.run( taskInstance, true );
```

**The reason** why we have two separate mechanisms that run here is that we want to differentiate between objects that exist and run on threads from those that run on the main thread.
Check the [Purpose of main thread](DesignAndUsage#Purpose_of_main_thread.md) section for details.


Therefore we need to for a **REQUIREMENT**:

_Each engine application should update the singleton instance of `TimeController` in its main application loop on the main thread._