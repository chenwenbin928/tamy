#pragma once

#include "core-Sound\SoundListener.h"


///////////////////////////////////////////////////////////////////////////////

class OpenALSoundSystem;

///////////////////////////////////////////////////////////////////////////////

/**
 * This is an OpenAL implementation of a 3d scene sound listener
 */
class OALSoundListener : public SoundListener
{
private:
   OpenALSoundSystem& m_soundSystem;

public:
   OALSoundListener(OpenALSoundSystem& soundSystem);

   void update();
};

///////////////////////////////////////////////////////////////////////////////
