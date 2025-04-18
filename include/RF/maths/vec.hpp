#pragma once

#include <cmath>
#include "RF/template.hpp" // RF_arithmetic_template
#include "RF/exception.hpp" // RF::runtime_error

namespace RF
{
	template<int n, typename A, RF_arithmetic_template(A)>
	struct vec;
}