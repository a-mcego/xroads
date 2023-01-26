#pragma once

namespace Xroads
{
	template<typename T>
	T lerp(const T& lhs, const T& rhs, float amount)
	{ return lhs*(1.0-amount)+rhs*amount; }
}
