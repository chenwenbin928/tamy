#include "core-AppFlow\Application.h"


///////////////////////////////////////////////////////////////////////////////

Application::Application(const std::string& name) 
      : m_name(name), 
      m_context(NULL),
      m_timeController(new TimeController())
{
}

///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
   delete m_timeController;
   m_timeController = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Application::update(float timeElapsed)
{
   m_timeController->update(timeElapsed);
}

///////////////////////////////////////////////////////////////////////////////