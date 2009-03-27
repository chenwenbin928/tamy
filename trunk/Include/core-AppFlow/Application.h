#pragma once

#include <stdexcept>
#include <string>


///////////////////////////////////////////////////////////////////////////////

class ExecutionContext;
class Renderer;
class ResourceManager;

///////////////////////////////////////////////////////////////////////////////

/**
 * An implementation of this class represents a single 'app'
 * that can be run by the flow framework.
 * An application can be thought of as a standalone program that 
 * does something, receives an input and uses system resources
 *
 */
class Application
{
public:
   enum Signal
   {
      ON_EXIT = 0x7fff
   };

private:
   std::string m_name;
   ExecutionContext* m_context;
  
public:
   virtual ~Application() {}

   const std::string& getName() const {return m_name;}
   bool operator==(const Application& rhs) const {return m_name == rhs.m_name;}
   bool operator!=(const Application& rhs) const {return !(*this == rhs);}
   bool operator<(const Application& rhs) const {return m_name < rhs.m_name;}

   /**
    * Method used by the ApplicationManager to inform the application
    * what's managing it and whom should it communicate its state to
    */
   void setContext(ExecutionContext& context) {m_context = &context;}

   /**
    * This method will be the first one to be called when
    * the application is scheduled to be ran
    */
   virtual void initialize(Renderer& renderer, ResourceManager& resourceManager) = 0;

   /**
    * This method is an application destructor - the class can't
    * do anything after this call is made - and it's bound to clean up after
    * itself in it
    */
   virtual void deinitialize() = 0;

   /**
    * This is the method that updates the internal state of the application
    */
   virtual void update(float timeElapsed) = 0;

protected:
   Application(const std::string& name) : m_name(name), m_context(NULL) {}

   ExecutionContext& context() 
   {
      if (m_context == NULL)
      {
         throw std::runtime_error("Application not embedded in an execution context");
      }

      return *m_context;
   }

   const ExecutionContext& context() const
   {
      if (m_context == NULL)
      {
         throw std::runtime_error("Application not embedded in an execution context");
      }

      return *m_context;
   }
};

///////////////////////////////////////////////////////////////////////////////
