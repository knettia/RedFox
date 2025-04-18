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
		constexpr vec<3, A>() : x(A()), y(A()), z(A()) { }
		constexpr vec<3, A>(A x, A y, A z) : x(x), y(y), z(z) { }

		template<typename T_x, typename T_y, typename T_z, RF_arithmetic_template(T_z), RF_arithmetic_template(T_x), RF_arithmetic_template(T_y)>
		constexpr vec<3, A>(T_x x, T_y y, T_z z)
		{
			this->x = static_cast<A>(x);
			this->y = static_cast<A>(y);
			this->z = static_cast<A>(z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator+(const vec<3, T> &v) const
		{
			return vec<3, A>(x + v.x, y + v.y, z + v.z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator+(const T &a) const
		{
			return vec<3, A>(x + a, y + a, z + a);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator-(const vec<3, T> &v) const
		{
			return vec<3, A>(x - v.x, y - v.y, z - v.z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator-(const T &a) const
		{
			return vec<3, A>(x - a, y - a, z - a);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator*(const vec<3, T> &v) const
		{
			return vec<3, A>(x * v.x, y * v.y, z * v.z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator*(const T s) const
		{
			return vec<3, A>(x * s, y * s, z * s);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator/(const vec<3, T> &v) const
		{
			return vec<3, A>(x / v.x, y / v.y, z / v.z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator/(const T s) const
		{
			return vec<3, A>(x / s, y / s, z / s);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator=(const vec<3, T> &v) const
		{
			return vec<3, A>(static_cast<A>(v.x), static_cast<A>(v.y), static_cast<A>(v.z));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<3, A> operator=(const T &a) const
		{
			return vec<3, A>(static_cast<A>(a), static_cast<A>(a), static_cast<A>(a));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const vec<3, T> &v) const
		{
			return (x == v.x) && (y == v.y) && (z == v.z);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const T &a) const
		{
			return (x == a) && (y == a) && (z == a);
		}

		// engine needs c++20 to compile anyway
		constexpr float length() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		// engine needs c++20 to compile anyway
		constexpr vec<3, A> unit() const
		{
			float l = length();
			return *this / l;
		}
	};

	using fvec3 =  vec<3, float>;
	using ivec3 =  vec<3, int>;
	using uivec3 = vec<3, unsigned int>;
	using dvec3 =  vec<3, double>;
}