#pragma once

#include <vector>
#include <array>
#include <utility>

namespace Xroads
{
    template<typename T, typename U>
    bool Contains(T& t, U& u)
    {
        for(auto& tt: t)
            if (tt == u)
                return true;
        return false;
    }

    template<typename CONTAINER>
    void SetToZero(CONTAINER& c)
    {
        for(auto& item: c)
            item = typename std::remove_reference<decltype(item)>::type();
    }
}
