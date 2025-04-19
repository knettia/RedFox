#pragma once

#include <cmath>
#include "RF/template.hpp" // RF_arithmetic_template
#include "RF/exception.hpp" // RF::runtime_error

namespace RF::detail
{
	template<int v>
	struct vec_guard
	{
		static constexpr bool valid = (v >= 2 && v <= 4);
	};
} // namespace RF::detail

namespace RF
{
	template<int n, typename A, RF_arithmetic_template(A)>
	requires(detail::vec_guard<n>::valid)
	struct vec;
} // namespace RF
