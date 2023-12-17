#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include <chrono>

#include "Types.h"

namespace Xroads
{
    struct CloqueVal
    {
        i64 n{};

        CloqueVal operator+(const CloqueVal& rhs) const { return {n+rhs.n}; }
        CloqueVal operator-(const CloqueVal& rhs) const { return {n-rhs.n}; }

        auto operator<=>(const CloqueVal&) const = default;
    };

    extern const CloqueVal starttime;

    inline CloqueVal Cloque()
    {
        return {std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()-starttime.n};
    }
}

#include "Logging.h"

namespace Xroads
{
    void BuildStackTrace();

    [[noreturn]] inline void Kill(std::string_view s)
    {
        Log(s);
        BuildStackTrace();
        std::abort();
    }
}

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
#include "Renderer.h"
#include "Shapes.h"
#include "Shaders.h"
#include "Textures.h"
#include "Algorithms.h"
#include "Text.h"

namespace Xroads
{
    void SelfTest();
}

#ifndef XROADS_NO_ABBREVIATION
namespace Xr = Xroads;
#endif
