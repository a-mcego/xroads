#pragma once

#include <cmath>
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
        Coord2D operator-() const { Coord2D ret = *this; ret.x = -x, ret.y = -y; return ret; }

        T Length() const
        {
            return sqrt(LengthSq());
        }

        T LengthSq() const
        {
            return x*x+y*y;
        }

        Coord2D Normalize() const
        {
            return (*this) / Length();
        }

        Coord2D Rotate(float angle_radians) const
        {
            Coord2D ret;
            ret.x = x*cos(angle_radians)-y*sin(angle_radians);
            ret.y = x*sin(angle_radians)+y*cos(angle_radians);
            return ret;
        }

        float ToAngle() const
        {
            return atan2(y, x);
        }
    };

    template<typename T>
    struct Coord3D
    {
        T x{}, y{}, z{};

#define implement_op(OP)\
     Coord3D& operator OP ## = (const Coord3D& rhs) { x OP ## = rhs.x, y OP ## = rhs.y, z OP ## = rhs.z; return *this; }\
     Coord3D operator OP (const Coord3D& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; } \
     Coord3D& operator OP ## = (const T& rhs) { x OP ## = rhs, y OP ## = rhs, z OP ## = rhs; return *this; }\
     Coord3D operator OP (const T& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; }

        implement_op(+);
        implement_op(-);
        implement_op(*);
        implement_op(/);

#undef implement_op
        Coord3D operator-() const { Coord3D ret = *this; ret.x = -x, ret.y = -y, ret.z = -z; return ret; }

        T Length() const
        {
            return std::sqrt(x*x+y*y+z*z);
        }

        T LengthSq() const
        {
            return x*x+y*y+z*z;
        }

        Coord3D Normalize() const
        {
            Coord3D t = *this;
            return t / Length();
        }

        Coord3D Cross(const Coord3D& rhs) const
        {
            return Coord3D
            {
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            };
        }
        T Dot(const Coord3D& rhs) const
        {
            return x*rhs.x+y*rhs.y+z*rhs.z;
        }

    };

    template<typename T>
    struct Rect2D
    {
        Coord2D<T> topleft, bottomright;

        bool is_inside(const Coord2D<T>& c) const
        {
            if (c.x < topleft.x or c.x > bottomright.x)
                return false;
            if (c.y < topleft.y or c.y > bottomright.y)
                return false;
            return true;
        }

        //TODO: rename this to be more descriptive
        Coord2D<T> GetAmount(const Coord2D<T>& amount)
        {
            return topleft*(-amount+1.0f) + bottomright*amount;
        }

        //TODO:
        Coord2D<T> GetRelativeCoords(const Coord2D<T>& coords)
        {
            return (coords-topleft)/(bottomright-topleft);
        }

        Coord2D<T> Center() const
        {
            return Coord2D<T>{Average(topleft.x,bottomright.x), Average(topleft.y,bottomright.y)};
        }

    };


    template<typename To, typename From> requires requires(From from, To to){ to.x=from.x; to.y=from.y; to.z=from.z; } To Conv3D(const From& from) { To to; to.x = from.x; to.y = from.y; to.z = from.z; return to; }
    template<typename T>
    T SwapYZ(const T& t)
    {
        T ret;
        ret.x = t.x;
        ret.y = t.z;
        ret.z = t.y;
        return ret;
    }



    using C2 = Coord2D<float>;
    using C3 = Coord3D<float>;
    using C2i = Coord2D<int>;
    using C3i = Coord3D<int>;

    template<typename T> requires requires(T t){ t.x; t.y; t.z; }
    C3 TriangleNormal(const T& p1, const T& p2, const T& p3)
    {
        C3 v1{p2.x-p1.x, p2.y-p1.y, p2.z-p1.z};
        C3 v2{p3.x-p1.x, p3.y-p1.y, p3.z-p1.z};
        return v1.Cross(v2).Normalize();
    }

}
