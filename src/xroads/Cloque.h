#pragma once

#include "xroads/Types.h"

#include <chrono>

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
