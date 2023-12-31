#pragma once

#include "Logging.h"

#include <string_view>
#include <cstdlib>

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
