#pragma once

#include <algorithm>
#include <optional>

namespace Xroads
{
    template<typename Container, typename UnaryPredicate>
    std::optional<typename Container::value_type> FindIf(const Container& container, UnaryPredicate predicate)
    {
        auto iter = std::find_if(container.begin(), container.end(), predicate);
        if (iter == container.end())
            return {};
        return *iter;
    }

    template<typename T>
    void AdvanceEnum(T& enum_val) requires std::is_enum<T>::value
    {
        enum_val = T(int(enum_val)+1);
        if (enum_val == T::N)
            enum_val = T(0);
    }
}
