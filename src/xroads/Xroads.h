#pragma once

#include "GlIncludes.h"
#include <chrono>

#include "Types.h"
#include "Cloque.h"
#include "Logging.h"

#include "Kill.h"
#include "XrAssert.h"
#include "Concepts.h"
#include "Strutil.h"
#include "Color.h"
#include "FileUtil.h"
#include "Coords.h"
#include "Mathutil.h"
#include "Textures.h"
#include "Cpputil.h"
#include "Containers.h"
#include "Containerutil.h"
#include "Random.h"
#include "IDutil.h"
#include "Sound.h"
#include "Renderer.h"
#include "Shapes.h"
#include "Shaders.h"
#include "Textures.h"
#include "Algorithms.h"
#include "Text.h"
#include "Screen.h"
#include "Input.h"
#include "Config.h"
#include "GUI.h"
#include "Engine.h"

namespace Xroads
{
    void SelfTest();
}

#ifndef XROADS_NO_ABBREVIATION
namespace Xr = Xroads;
#endif
