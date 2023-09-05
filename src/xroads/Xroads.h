#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <chrono>

#include "XrAssert.h"
#include "Concepts.h"
#include "Types.h"
#include "Color.h"
#include "FileUtil.h"
#include "Coords.h"
#include "Mathutil.h"
#include "Textures.h"
#include "Cpputil.h"
#include "Strutil.h"
#include "Containers.h"
#include "Containerutil.h"
#include "Random.h"
#include "IDutil.h"
#include "Renderer.h"
#include "Shapes.h"
#include "Shaders.h"
#include "Textures.h"

namespace Xroads
{
    [[noreturn]] inline void Kill([[maybe_unused]] const std::string& s)
    {
#ifdef XR_VERBOSE_ERRORS
        std::cerr << s << std::endl;
#endif // XR_VERBOSE_ERRORS
        std::abort();
    }

    inline i64 Cloque()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
}

namespace Xr = Xroads; //comment out if the short name conflicts with something
