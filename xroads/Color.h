#pragma once

namespace Xroads
{
	struct Color { float r{}, g{}, b{}, a{}; };

	inline Color operator*(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r*rhs.r, lhs.g*rhs.g, lhs.b*rhs.b, lhs.a*rhs.a}; }

	inline Color operator*(const Color& lhs, float rhs)
	{ return Color{lhs.r*rhs, lhs.g*rhs, lhs.b*rhs, lhs.a*rhs}; }

	inline Color operator+(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r+rhs.r, lhs.g+rhs.g, lhs.b+rhs.b, lhs.a+rhs.a}; }

	inline Color operator-(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r-rhs.r, lhs.g-rhs.g, lhs.b-rhs.b, lhs.a-rhs.a}; }
}

