#pragma once

#include "RF/vec.hpp"

namespace RF
{
	template<typename A>
	struct alignas(16) vec<3, A>
	{
	public:
		A x;
		A y;
		A z;
	private:
		A _padding; // private padding
	public:
		vec() : x(A()), y(A()), z(A())
		{
		}

		vec(A x, A y, A z) : x(x), y(y), z(z)
		{
		}
	};

	using fvec3 =  vec<3, float>;
	using ivec3 =  vec<3, int>;
	using uivec3 = vec<3, unsigned int>;
	using dvec3 =  vec<3, double>;
}