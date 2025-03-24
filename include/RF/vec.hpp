#pragma once

#include <cmath>
#include "RF/template.hpp" // RF_arithmetic_template

namespace RF
{
	template<int n, typename A, RF_arithmetic_template(A)>
	struct vec;
}