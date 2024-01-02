#pragma once

#include "Types.h"

namespace Xroads
{
    template<typename T> T Min(const T& a, const T& b) { return (a<b?a:b); }
    template<typename T> T Max(const T& a, const T& b) { return (a>b?a:b); }

    template<typename T>
    T Lerp(const T& lhs, const T& rhs, f32 amount)
    { return lhs*(1.0-amount)+rhs*amount; }

    template <typename T> i32 Sign(T val)
    { return (T{} < val) - (val < T{}); }

    template<typename T> T Abs(const T& val) { if (val<T()) return -val; return val; }


    template<typename T>
    T Average(const T& a, const T& b)
    {
        return (a+b)*0.5f;
    }

    template<typename T>
    T Clamp(const T& val, const T& low, const T& high)
    {
        if (val<low) return low;
        if (val>high) return high;
        return val;
    }

    template<typename T>
    bool IsBetween(const T& val, const T& low, const T& high)
    {
        if (val<low) return false;
        if (val>high) return false;
        return true;
    }

    //TODO: add documentation on how this class works and how it's supposed to be used
    template<typename T>
    struct Damped
    {
        f32 damping{};
        T target{};

        void Update()
        {
            target *= damping;
        }

        T Get()
        {
            T ret = target*(1.0f-damping);
            return ret;
        }
        void Reset() { target = T{}; }

        Damped& operator+=(const T& t) { target += t; return *this; }
        Damped& operator-=(const T& t) { target -= t; return *this; }
    };

    template<typename T>
    struct TargetValue
    {
        f32 damping{};
        T target{};
        T current_value{};

        void Update()
        {
            current_value = target*(1.0f-damping)+current_value*damping;
        }

        T Get()
        {
            return current_value;
        }
        void Reset() { target = T{}; }

        TargetValue& operator+=(const T& t) { target += t; return *this; }
        TargetValue& operator-=(const T& t) { target -= t; return *this; }
    };


    template<typename T>
    T FastCos(T input)
    {
        return std::cos(input);
    }
    template<typename T>
    T FastSin(T input)
    {
        return std::sin(input);
    }

    template<typename T>
    T Square(T input)
    {
        return input*input;
    }
}
