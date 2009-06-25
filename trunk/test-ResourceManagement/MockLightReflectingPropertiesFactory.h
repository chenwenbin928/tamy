#pragma once

#include "core-ResourceManagement\LightReflectingPropertiesFactory.h"
#include "LightReflectingPropertiesStub.h"


//////////////////////////////////////////////////////////////////////////////

struct Mock;

//////////////////////////////////////////////////////////////////////////////

LightReflectingProperties* LightReflectingPropertiesFactory<Mock>::operator()()
{
   return new LightReflectingPropertiesStub();
}

//////////////////////////////////////////////////////////////////////////////
