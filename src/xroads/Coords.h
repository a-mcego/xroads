#pragma once

#include <cmath>
#include <array>
#include <iostream>
#include "Mathutil.h"

namespace Xroads
{
    template<typename T>
    struct Coord2D
    {
        T x{}, y{};
#define implement_op(OP)\
     constexpr Coord2D& operator OP ## = (const Coord2D& rhs) { x OP ## = rhs.x, y OP ## = rhs.y; return *this; }\
     constexpr Coord2D operator OP (const Coord2D& rhs) const { Coord2D ret = *this; ret OP ## = rhs; return ret; } \
     constexpr Coord2D& operator OP ## = (const T& rhs) { x OP ## = rhs, y OP ## = rhs; return *this; }\
     constexpr Coord2D operator OP (const T& rhs) const { Coord2D ret = *this; ret OP ## = rhs; return ret; }

        implement_op(+);
        implement_op(-);
        implement_op(*);
        implement_op(/);
        implement_op(%);

#undef implement_op
        constexpr Coord2D operator-() const { Coord2D ret = *this; ret.x = -x, ret.y = -y; return ret; }

        constexpr auto operator<=>(const Coord2D& rhs) const = default;

        constexpr T Length() const
        {
            return sqrt(LengthSq());
        }

        constexpr T LengthSq() const
        {
            return x*x+y*y;
        }

        constexpr T Dot(const Coord2D& rhs) const
        {
            return x*rhs.x+y*rhs.y;
        }

        constexpr T NormalizedDot(const Coord2D& rhs)
        {
            return Normalize().Dot(rhs.Normalize());
        }

        constexpr Coord2D Normalize() const
        {
            return (*this) / Length();
        }

        constexpr Coord2D Project(Coord2D projector)
        {
            return projector*(Dot(projector)/projector.Dot(projector));
        }

        constexpr Coord2D Reject(Coord2D projector)
        {
            return (*this)-Project(projector);
        }

        constexpr Coord2D NormalizeOrDefault() const
        {
            auto len = LengthSq();
            if (len == 0.0f)
                return Coord2D{1.0f,0.0f};
            Coord2D t = *this;
            return t / sqrt(len);
        }

        constexpr Coord2D Rotate(float angle_radians) const
        {
            Coord2D ret;
            ret.x = x*cos(angle_radians)-y*sin(angle_radians);
            ret.y = x*sin(angle_radians)+y*cos(angle_radians);
            return ret;
        }

        constexpr float ToAngle() const
        {
            return atan2(y, x);
        }
    };

    template<typename T>
    struct Coord3D
    {
        T x{}, y{}, z{};

#define implement_op(OP)\
     constexpr Coord3D& operator OP ## = (const Coord3D& rhs) { x OP ## = rhs.x, y OP ## = rhs.y, z OP ## = rhs.z; return *this; }\
     constexpr Coord3D operator OP (const Coord3D& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; } \
     constexpr Coord3D& operator OP ## = (const T& rhs) { x OP ## = rhs, y OP ## = rhs, z OP ## = rhs; return *this; }\
     constexpr Coord3D operator OP (const T& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; }

        implement_op(+);
        implement_op(-);
        implement_op(*);
        implement_op(/);

#undef implement_op
        constexpr Coord3D operator-() const { Coord3D ret = *this; ret.x = -x, ret.y = -y, ret.z = -z; return ret; }

        constexpr auto operator<=>(const Coord3D& rhs) const = default;

        constexpr Coord2D<T> xy() const { return Coord2D<T>{x,y}; }
        constexpr Coord2D<T> xz() const { return Coord2D<T>{x,z}; }
        constexpr Coord2D<T> yx() const { return Coord2D<T>{y,x}; }
        constexpr Coord2D<T> yz() const { return Coord2D<T>{y,z}; }
        constexpr Coord2D<T> zx() const { return Coord2D<T>{z,x}; }
        constexpr Coord2D<T> zy() const { return Coord2D<T>{z,y}; }

        constexpr Coord3D<T> xyz() const { return Coord3D<T>{x,y,z}; }
        constexpr Coord3D<T> xzy() const { return Coord3D<T>{x,z,y}; }
        constexpr Coord3D<T> yxz() const { return Coord3D<T>{y,x,z}; }
        constexpr Coord3D<T> yzx() const { return Coord3D<T>{y,z,x}; }
        constexpr Coord3D<T> zxy() const { return Coord3D<T>{z,x,y}; }
        constexpr Coord3D<T> zyx() const { return Coord3D<T>{z,y,x}; }

        constexpr T Length() const
        {
            return std::sqrt(x*x+y*y+z*z);
        }

        constexpr T LengthSq() const
        {
            return x*x+y*y+z*z;
        }

        constexpr Coord3D Normalize() const
        {
            Coord3D t = *this;
            return t / Length();
        }

        constexpr Coord3D NormalizeOrDefault() const
        {
            auto len = LengthSq();
            if (len == 0.0f)
                return Coord3D{1.0f,0.0f,0.0f};
            Coord3D t = *this;
            return t / sqrt(len);
        }

        constexpr Coord3D Cross(const Coord3D& rhs) const
        {
            return Coord3D
            {
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            };
        }
        constexpr T Dot(const Coord3D& rhs) const
        {
            return x*rhs.x+y*rhs.y+z*rhs.z;
        }

        constexpr T NormalizedDot(const Coord3D& rhs)
        {
            return Normalize().Dot(rhs.Normalize());
        }
    };

    template<typename T>
    struct Coord4D
    {
        T x{}, y{}, z{}, w{};

#define implement_op(OP)\
     constexpr Coord4D& operator OP ## = (const Coord4D& rhs) { x OP ## = rhs.x, y OP ## = rhs.y, z OP ## = rhs.z; w OP ## = rhs.w; return *this; }\
     constexpr Coord4D operator OP (const Coord4D& rhs) const { Coord4D ret = *this; ret OP ## = rhs; return ret; } \
     constexpr Coord4D& operator OP ## = (const T& rhs) { x OP ## = rhs, y OP ## = rhs, z OP ## = rhs; w OP ## = rhs; return *this; }\
     constexpr Coord4D operator OP (const T& rhs) const { Coord4D ret = *this; ret OP ## = rhs; return ret; }

        implement_op(+);
        implement_op(-);
        implement_op(*);
        implement_op(/);

#undef implement_op
        constexpr Coord4D operator-() const { Coord4D ret = *this; ret.x = -x, ret.y = -y, ret.z = -z; ret.w = -w; return ret; }

        constexpr auto operator<=>(const Coord4D& rhs) const = default;

        constexpr T Length() const
        {
            return std::sqrt(x*x+y*y+z*z+w*w);
        }

        constexpr T LengthSq() const
        {
            return x*x+y*y+z*z+w*w;
        }

        constexpr Coord4D Normalize() const
        {
            Coord4D t = *this;
            return t / Length();
        }

        constexpr Coord4D NormalizeOrDefault() const
        {
            auto len = LengthSq();
            if (len == 0.0f)
                return Coord4D{0.0f,0.0f,0.0,1.0f};
            Coord4D t = *this;
            return t / sqrt(len);
        }

        constexpr T Dot(const Coord4D& rhs) const
        {
            return x*rhs.x+y*rhs.y+z*rhs.z+w*rhs.w;
        }

        constexpr T NormalizedDot(const Coord4D& rhs)
        {
            return Normalize().Dot(rhs.Normalize());
        }
    };

    template<typename T>
    struct Mat4
    {
        std::array<T,16> data{}; //column-major!




    };

    template<typename T>
    struct Rect2D
    {
        Coord2D<T> topleft, bottomright;

        static Rect2D MakeBoundingBox(const Coord2D<T>& coord1, const Coord2D<T>& coord2)
        {
            Rect2D ret{coord1,coord1};

            ret.topleft.x = Min(ret.topleft.x, coord2.x);
            ret.topleft.y = Min(ret.topleft.y, coord2.y);

            ret.bottomright.x = Max(ret.bottomright.x, coord2.x);
            ret.bottomright.y = Max(ret.bottomright.y, coord2.y);

            return ret;
        }

        bool is_inside(const Coord2D<T>& c) const
        {
            if (c.x < topleft.x or c.x > bottomright.x)
                return false;
            if (c.y < topleft.y or c.y > bottomright.y)
                return false;
            return true;
        }

        //TODO: rename these to be more descriptive
        //transforms from the internal coordinates of the rect to the outside coordinates
        Coord2D<T> TransformFrom(const Coord2D<T>& relative) const
        {
            return topleft*(-relative+1.0f) + bottomright*relative;
        }
        //transforms from the outside coordinates to the internal coordinates
        Coord2D<T> TransformTo(const Coord2D<T>& absolute) const
        {
            return (absolute-topleft)/(bottomright-topleft);
        }
        Coord2D<T> Center() const
        {
            return Coord2D<T>{Average(topleft.x,bottomright.x), Average(topleft.y,bottomright.y)};
        }

        Coord2D<T> GetTopLeft() const
        {
            return topleft;
        }
        Coord2D<T> GetTopRight() const
        {
            return {bottomright.x, topleft.y};
        }
        Coord2D<T> GetBottomLeft() const
        {
            return {topleft.x, bottomright.y};
        }
        Coord2D<T> GetBottomRight() const
        {
            return bottomright;
        }

    };

    //bjarne says "requires requires" is a code smell
    template<typename To, typename From> requires requires(From from, To to){ to.x=from.x; to.y=from.y; to.z=from.z; }
    To Conv3D(const From& from)
    {
        To to; to.x = from.x; to.y = from.y; to.z = from.z; return to;
    }

    template<typename T>
    T SwapYZ(const T& t)
    {
        T ret;
        ret.x = t.x;
        ret.y = t.z;
        ret.z = t.y;
        return ret;
    }

    using C2 = Coord2D<f32>;
    using C3 = Coord3D<f32>;
    using C4 = Coord4D<f32>;

    using C2i = Coord2D<i32>;
    using C3i = Coord3D<i32>;
    using C4i = Coord4D<i32>;

    using C2u = Coord2D<u32>;
    using C3u = Coord3D<u32>;
    using C4u = Coord4D<u32>;

    template<typename T> requires requires(T t){ t.x; t.y; t.z; }
    C3 TriangleNormal(const T& p1, const T& p2, const T& p3)
    {
        C3 v1{p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
        C3 v2{p3.x-p1.x, p3.y-p1.y, p3.z-p1.z};
        return -v1.Cross(v2).Normalize();
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& o, const Xroads::Coord2D<T>& c)
    {
        o << '(' << c.x << ' ' << c.y << ')';
        return o;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& o, const Xroads::Coord3D<T>& c)
    {
        o << '(' << c.x << ' ' << c.y << ' ' << c.z << ')';
        return o;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& o, const Xroads::Coord4D<T>& c)
    {
        o << '(' << c.x << ' ' << c.y << ' ' << c.z << ' ' << c.w << ')';
        return o;
    }
}
