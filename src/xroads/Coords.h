#pragma once

#include <cmath>

namespace Xroads
{

	template<typename T>
	struct Coord2D
	{
		T y=0, x=0;

		Coord2D() = default;
		Coord2D(const T rhs) { y = rhs, x = rhs; }
		Coord2D(const T rhs1, const T rhs2) { y = rhs1, x = rhs2; }

		Coord2D& operator=(const Coord2D& rhs) = default;
		Coord2D& operator=(const T rhs) { y = rhs, x = rhs; }

#define implement_op(OP)\
	 Coord2D& operator OP ## = (const Coord2D& rhs) { y OP ## = rhs.y, x OP ## = rhs.x; return *this; }\
	 Coord2D operator OP (const Coord2D& rhs) const { Coord2D ret = *this; ret OP ## = rhs; return ret; } \
	 Coord2D& operator OP ## = (const T& rhs) { y OP ## = rhs, x OP ## = rhs; return *this; }\
	 Coord2D operator OP (const T& rhs) const { Coord2D ret = *this; ret OP ## = rhs; return ret; }

		implement_op(+);
		implement_op(-);
		implement_op(*);
		implement_op(/);
		implement_op(%);

#undef implement_op
		Coord2D operator-() { Coord2D ret = *this; ret.y = -y, ret.x = -x; return ret; }

		T Length() const
		{
			return sqrt(y*y+x*x);
		}

		T LengthSq() const
		{
			return y*y+x*x;
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
		T z=0, y=0, x=0;
        Coord3D Normalize() const
        {
            Coord3D t = *this;
            t /= Length();
            return t;
        }
        T Length() const
        {
            return sqrt(z*z+y*y+x*x);
        }

        T LengthSq() const
        {
            return z*z+y*y+x*x;
        }

        Coord3D Normalize()
		{
			Coord3D t = *this;
			T length = std::sqrt(x*x+y*y+z*z);
			t /= length;
			return t;
		}

#define implement_op(OP)\
	 Coord3D& operator OP ## = (const Coord3D& rhs) { z OP ## = rhs.z, y OP ## = rhs.y, x OP ## = rhs.x; return *this; }\
	 Coord3D operator OP (const Coord3D& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; } \
	 Coord3D& operator OP ## = (const T& rhs) { z OP ## = rhs, y OP ## = rhs, x OP ## = rhs; return *this; }\
	 Coord3D operator OP (const T& rhs) const { Coord3D ret = *this; ret OP ## = rhs; return ret; }

		implement_op(+);
		implement_op(-);
		implement_op(*);
		implement_op(/);

#undef implement_op
		Coord3D operator-() const { Coord3D ret = *this; ret.z = -z, ret.y = -y, ret.x = -x; return ret; }



		Coord3D cross(const Coord3D& rhs)
		{
			return Coord3D
			{
				x * rhs.y - y * rhs.x,
				z * rhs.x - x * rhs.z,
				y * rhs.z - z * rhs.y
			};
		}
        float Dot(const Coord3D& rhs)
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
			if (c.y < topleft.y or c.y > bottomright.y)
				return false;
			if (c.x < topleft.x or c.x > bottomright.x)
				return false;
			return true;
		}

		Coord2D<T> GetAmount(const Coord2D<T>& amount)
		{
            return topleft*(-amount+1.0f) + bottomright*amount;
		}


		Coord2D<T> Center() const
		{
			return Coord2D<T>{Average(topleft.y,bottomright.y), Average(topleft.x,bottomright.x)};
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
}
