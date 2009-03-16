#include "core\Timer.h"
#include <windows.h>
#include <mmsystem.h>


///////////////////////////////////////////////////////////////////////////////

CTimer::CTimer(void)
      : m_frameRate(0),
      m_fpsFrameCount(0),
	   m_fpsTimeElapsed(0)
{
   __int64 freq;
   QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
   m_timeScale = 1.f / freq;

   QueryPerformanceCounter((LARGE_INTEGER*)&m_lastTime);
   m_timeElapsed = 0;
}

///////////////////////////////////////////////////////////////////////////////

CTimer::~CTimer(void)
{
}

///////////////////////////////////////////////////////////////////////////////

void CTimer::tick(void)
{
   __int64 currTime;
   QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

   m_timeElapsed = (currTime - m_lastTime) * m_timeScale;
   m_lastTime = currTime;

   // Calculate Frame Rate
	m_fpsFrameCount++;
	m_fpsTimeElapsed += m_timeElapsed;
	if (m_fpsTimeElapsed > 1.0f) 
    {
		m_frameRate			= m_fpsFrameCount;
		m_fpsFrameCount   = 0;
		m_fpsTimeElapsed	= 0.0f;
	}
}

///////////////////////////////////////////////////////////////////////////////

float CTimer::getTimeElapsed() const
{
   return m_timeElapsed;
}

///////////////////////////////////////////////////////////////////////////////

double CTimer::getCurrentTime() const
{
   __int64 currTime;
   QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

   return (double)currTime * m_timeScale;
}

///////////////////////////////////////////////////////////////////////////////

unsigned long CTimer::getFrameRate() const
{
   return m_frameRate;
}

///////////////////////////////////////////////////////////////////////////////
