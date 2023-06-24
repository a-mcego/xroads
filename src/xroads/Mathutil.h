#pragma once

namespace Xroads
{
	template<typename T> T Min(const T& a, const T& b) { return (a<b?a:b); }
	template<typename T> T Max(const T& a, const T& b) { return (a>b?a:b); }

	template<typename T>
	T Lerp(const T& lhs, const T& rhs, float amount)
	{ return lhs*(1.0-amount)+rhs*amount; }

	template <typename T> int Sign(T val)
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
    struct Damped
    {
        float damping{};
        T target{};

        T Get()
        {
            T ret = target*(1.0f-damping);
            target *= damping;
            return ret;
        }
        void Reset() { target = T{}; }

        Damped& operator+=(const T& t) { target += t; return *this; }
        Damped& operator-=(const T& t) { target -= t; return *this; }
    };
}
