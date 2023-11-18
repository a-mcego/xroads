#pragma once

namespace Xroads
{
    template<typename T>
    struct ColorType
    {
        T r{}, g{}, b{}, a{};
        auto operator<=>(const ColorType&) const = default; //yeah, equality checking with floats is iffy but w/e

        operator ColorType<float>() const requires (!std::same_as<T,float>)
        {
            return ColorType<float>{r,g,b,a};
        }
#if defined(__STDCPP_BFLOAT16_T__)
        operator ColorType<bf16>() const requires (!std::same_as<T,bf16>)
        {
            return ColorType<bf16>{bf16(r),bf16(g),bf16(b),bf16(a)};
        }
#endif

    };

    template<typename T>
    inline ColorType<T> operator*(const ColorType<T>& lhs, const ColorType<T>& rhs)
    { return ColorType<T>{lhs.r*rhs.r, lhs.g*rhs.g, lhs.b*rhs.b, lhs.a*rhs.a}; }

    template<typename T>
    inline ColorType<T> operator*(const ColorType<T>& lhs, f32 rhs)
    { return ColorType<T>{T(f32(lhs.r)*rhs), T(f32(lhs.g)*rhs), T(f32(lhs.b)*rhs), T(f32(lhs.a)*rhs)}; }

    template<typename T>
    inline ColorType<T>& operator*=(ColorType<T>& lhs, const ColorType<T>& rhs)
    { lhs.r*=rhs.r; lhs.g*=rhs.g; lhs.b*=rhs.b; lhs.a*=rhs.a; return lhs; }

    template<typename T>
    inline ColorType<T>& operator*=(ColorType<T>& lhs, f32 rhs)
    { lhs.r*=rhs; lhs.g*=rhs; lhs.b*=rhs; lhs.a*=rhs; return lhs; }

    template<typename T>
    inline ColorType<T> operator+(const ColorType<T>& lhs, const ColorType<T>& rhs)
    { return ColorType<T>{lhs.r+rhs.r, lhs.g+rhs.g, lhs.b+rhs.b, lhs.a+rhs.a}; }

    template<typename T>
    inline ColorType<T> operator-(const ColorType<T>& lhs, const ColorType<T>& rhs)
    { return ColorType<T>{lhs.r-rhs.r, lhs.g-rhs.g, lhs.b-rhs.b, lhs.a-rhs.a}; }

    using Color = ColorType<f32>;
    using Color_bf = ColorType<bf16>;

    const Color COLOR_WHITE = Color{1.0f,1.0f,1.0f,1.0f};
    const Color COLOR_BLACK = Color{0.0f,0.0f,0.0f,1.0f};
#if defined(__STDCPP_BFLOAT16_T__)
    const Color_bf COLOR_BF_WHITE = Color_bf{1.0bf16,1.0bf16,1.0bf16,1.0bf16};
    const Color_bf COLOR_BF_BLACK = Color_bf{0.0bf16,0.0bf16,0.0bf16,1.0bf16};
#else
    const auto COLOR_BF_WHITE = COLOR_WHITE;
    const auto COLOR_BF_BLACK = COLOR_BLACK;
#endif // defined
}
