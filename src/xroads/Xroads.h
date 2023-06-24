#ifndef HEADER_6CD078F66E36CC64
#define HEADER_6CD078F66E36CC64

#pragma once

#include "Types.h"
#include "Color.h"
#include "Mathutil.h"
#include "Coords.h"
#include "Textures.h"
#include "Cpputil.h"
#include "Strutil.h"
#include "Containers.h"
#include "Containerutil.h"
#include "Random.h"
#include "IDutil.h"

namespace Xroads
{
	[[noreturn]] inline void KILL([[maybe_unused]] const std::string& s)
	{
#ifdef XR_VERBOSE_ERRORS
		std::cerr << s << std::endl;
#endif // XR_VERBOSE_ERRORS
		std::abort();
	}
}


namespace Xr = Xroads; //comment out if the short name conflicts with something
#endif // header guard 

