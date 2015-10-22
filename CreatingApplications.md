# Creating applications #

If you're not using any external system that manages the main application loop, you may want to use what Tamy's got in store for you.


Application management system consists of two components:
  * `Application` - a class that represents a piece of your game
  * `ApplicationManager` - a platform independent entity that manages `Applications`.

You can think of individual `Applications` as screens in your game - each represents some different aspect of your game. There will be a small application handling the menu, and another one handling the gameplay etc.

The `ApplicationManager` allows you to combine those tiny bits into a state machine that can be easily controlled. For instance, you'll be able to freeze one screen and activate the other.

## Application ##

Particular `Applications` receive messages that tell them what the manager expects them to do at any given moment ( resource management wise ):

```
 /**
    * This method will be the first one to be called when
    * the application is scheduled to be ran
    */
   virtual void initialize() = 0;

   /**
    * This method is an application destructor - the class can't
    * do anything after this call is made - and it's bound to clean up after
    * itself in it
    */
   virtual void deinitialize() = 0;

   /**
    * This method will be called as soon as an application 
    * enters the hibernation mode.
    * In this mode, the app can keep all its resources, however it is supposed
    * to release common resources, such as the renderer etc.
    */
   virtual void hibernate() = 0;

   /**
   * This method will be called as soon as an application 
   * exits the hibernation mode.
   * When this call happens, the application is free to reclaim
   * common resources, such as the renderer etc.
   */
   virtual void dehibernate() = 0;

   /**
    * When this method is called, it means that some other active application
    * is sending us a signal.
    */
   virtual void notify(const std::string& senderApp, int signalCode) = 0;
```

The `notify` method receives notifications sent using one of the `ApplicationManager::signal` methods.

```
void signal(const Application& app, int signalId);

void signal(const Application& app, const std::string& receiverApp, int signalId);
```

## Application names ##

Each application requires you to name it in its constructor. Those names will later on be used as state identifiers whenever you wish to change the active application or send a message through the `ApplicationManager` from one app to another.

The names **HAVE TO BE UNIQUE**.

## Application manager ##

Application manager manages particular applications, organizing them into a state machine, where each applications become states.

You can add the application using the following method:
```
/**
 * Adds a new application to be ran
 */
void addApplication(Application& app);
```

Once you add all applications, you need to specify which one will be run first:

```
/**
 * This method selects which application should be ran first
 */
void setEntryApplication(const std::string& appName);
```

## Main loop ##

There is no such thing as an explicit main application loop.
Instead, each `Application` contains a dedicated `TimeController` instance, that you can add your `Updatables` to.

To access it, simply call this method:

```
/**
 * If you want the application to run any code, you need to add it to
 * the time controller of that application. 
 * This method gives you the access to the controller
 */
TimeController& timeController();
```

## Example ##

And here's a small example of how you can configure and run the most basic game composed of just one `Application`:

```
MAIN_FUNCTION_START()
{
   const uint windowWidth = 800;
   const uint windowHeight = 600;
   const bool fullScreen = false;
   const char* appName = "WinGame";

   // create the main application window
   WindowHandle mainWindow = WindowBuilder::createWindow( appName, fullScreen, windowWidth, windowHeight );


   // create the application and plug it into the manager
   GameApplication* application = new GameApplication( windowWidth, windowHeight, fullScreen, mainWindow );
   ApplicationManager& appMgr = WindowBuilder::getApplicationManager();
   appMgr.addApplication( *application );
   appMgr.setEntryApplication( application->getName() );

   // run the main loop
   while( appMgr.step() )
   {
   }

   // cleanup
   delete application;

   return 0;
}
MAIN_FUNCTION_END()
```