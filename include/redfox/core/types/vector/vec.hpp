#pragma once

#include <cmath>
#include "redfox/core/utils/template_utilities.hpp" // RF_arithmetic_template

namespace RF
{
	template<int n, typename A, RF_arithmetic_template(A)>
	struct vec;
}