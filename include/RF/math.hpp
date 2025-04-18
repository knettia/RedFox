#pragma once

#include <cmath>
#include "RF/template.hpp"

namespace RF
{
	constexpr long double pi_value = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648;

	template<typename A, RF_arithmetic_template(A)>
	constexpr A pi(std::uint8_t digits = 16)
	{
		double scale = std::pow(10.0, digits);
		return static_cast<A>(std::round(RF::pi_value * scale) / scale);
	}

	template<typename A, typename D, RF_arithmetic_template(A), RF_arithmetic_template(D)>
	constexpr A radians(D degrees)
	{
		return static_cast<A>(degrees) * RF::pi<A>(16) / static_cast<A>(180.00);
	}
}
