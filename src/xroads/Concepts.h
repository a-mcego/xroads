#pragma once

#include <type_traits>

namespace Xroads
{
    //general concepts
    template <typename... Args>
    concept AllSameType = std::conjunction_v<std::is_same<Args, typename std::tuple_element<0, std::tuple<Args...>>::type>...>;

    template<typename T, typename... AllowedTypes>
    concept IsOneOf = std::disjunction_v<std::is_same<T, AllowedTypes>...>;

    struct TextureDef;

    struct CountAggregateMembers_InternalType { template<typename T> operator T() {} };

    template<typename T>
    consteval auto CountAggregateMembers(auto ...Members) requires std::is_aggregate_v<T>
    {
        if constexpr(requires { T{ Members... }; })
            return CountAggregateMembers<T>(Members..., CountAggregateMembers_InternalType{});
        else
            return sizeof...(Members) - 1;
    }

    template<typename T, size_t N>
    concept HasNAggregateMembers = (CountAggregateMembers<T>()==N);


    //rendering concepts
    template<typename Shape>
    concept IsShape = requires(const Shape& q, const TextureDef& sprite) { q.GetVertices(sprite); };

    template<typename T> concept Bool = std::same_as<T,bool>;

    template<typename T> concept Char = std::same_as<T,char>;

    template<typename T> concept I8 = std::same_as<T,i8>;
    template<typename T> concept I16 = std::same_as<T,i16>;
    template<typename T> concept I32 = std::same_as<T,i32>;
    template<typename T> concept I64 = std::same_as<T,i64>;

    template<typename T> concept U8 = std::same_as<T,u8>;
    template<typename T> concept U16 = std::same_as<T,u16>;
    template<typename T> concept U32 = std::same_as<T,u32>;
    template<typename T> concept U64 = std::same_as<T,u64>;

    template<typename T> concept F32 = std::same_as<T,f32>;
    template<typename T> concept F64 = std::same_as<T,f64>;

    template<typename T> concept String = std::same_as<T,std::string>;
}
