#pragma once

#include <cmath>
#include "RF/template.hpp" // RF_arithmetic_template
#include "RF/exception.hpp" // RF::runtime_error

namespace RF::detail
{
	template<int n, int m>
	struct mat_guard
	{
		static constexpr bool valid = (n >= 2 && n <= 4) && (m >= 2 && m <= 4);
	};
} // namespace RF::detail

namespace RF
{
	template<int n, int m, typename A, RF_arithmetic_template(A)>
	requires(RF::detail::mat_guard<n, m>::valid)
	struct mat;
} // namespace RF
