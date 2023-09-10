#pragma once

#include <type_traits>

namespace Xroads
{
    //general concepts
    template <typename... Args>
    concept AllSameType = std::conjunction_v<std::is_same<Args, typename std::tuple_element<0, std::tuple<Args...>>::type>...>;

    template<typename T, typename... AllowedTypes>
    concept IsOneOf = std::disjunction_v<std::is_same<T, AllowedTypes>...>;

    struct Sprite;

    //rendering concepts
    template<typename QuadType>
    concept IsQuadType = requires(const QuadType& q, const Sprite& sprite) { q.GetVertices(sprite);  };
}
