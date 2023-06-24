#pragma once

namespace Xroads
{
	struct Color
	{
		float r{}, g{}, b{}, a{};

		auto operator<=>(const Color&) const = default; //yeah, equality checking with floats is iffy but w/e
	};

	inline Color operator*(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r*rhs.r, lhs.g*rhs.g, lhs.b*rhs.b, lhs.a*rhs.a}; }

	inline Color operator*(const Color& lhs, float rhs)
	{ return Color{lhs.r*rhs, lhs.g*rhs, lhs.b*rhs, lhs.a*rhs}; }

	inline Color operator+(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r+rhs.r, lhs.g+rhs.g, lhs.b+rhs.b, lhs.a+rhs.a}; }

	inline Color operator-(const Color& lhs, const Color& rhs)
	{ return Color{lhs.r-rhs.r, lhs.g-rhs.g, lhs.b-rhs.b, lhs.a-rhs.a}; }

	const Color COLOR_WHITE = Color{1.0f,1.0f,1.0f,1.0f};
	const Color COLOR_BLACK = Color{0.0f,0.0f,0.0f,1.0f};
}

