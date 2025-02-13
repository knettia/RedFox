#pragma once

#include "./vec.hpp"

namespace RF
{
	template<typename A>
	struct alignas(8) vec<2, A>
	{
		A x;
		A y;

		constexpr vec() : x(A()), y(A()) { }
		constexpr vec(A x, A y) : x(x), y(y) { }

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec(T x, T y)
		{
			this->x = static_cast<A>(x);
        		this->y = static_cast<A>(y);
		}

		constexpr vec operator+(const vec &v) const
		{
			return vec(x + v.x, y + v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec operator+(const T &a) const
		{
			return vec(x + a, y + a);
		}
		
		constexpr vec operator-(const vec &v) const
		{
			return vec(x - v.x, y - v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec operator-(const T &a) const
		{
			return vec(x - a, y - a);
		}

		constexpr vec operator*(const vec &v) const
		{
			return vec(x * v.x, y * v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec operator*(const T s) const
		{
			return vec(x * s, y * s);
		}

		constexpr vec operator/(const vec &v) const
		{
			return vec(x / v.x, y / v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec operator/(const T s) const
		{
			return vec(x / s, y / s);
		}
		
		// engine needs c++20 to compile anyway
		constexpr float length() const
		{
			return std::sqrt(x * x + y * y);
		}
		
		// engine needs c++20 to compile anyway
		constexpr vec unit() const
		{
			float l = length();
			return *this / l;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr T dot(const vec &v) const
		{
			return static_cast<T>(x) * v.x + static_cast<T>(y) * v.y;
		}

		constexpr vec perp() const
		{
			return vec(-y, x);
		}

		constexpr vec hadamard(const vec &v) const
		{
			return vec(x * v.x, y * v.y);
		}

		constexpr vec project(const vec &b) const
		{
			A b_len_sq = b.dot(b);
			if (b_len_sq == 0) return vec(0, 0);
			A scale = this->dot<A>(b) / b_len_sq;
			return b * scale;
		}

		constexpr vec reject(const vec &b) const
		{
			return *this - this->project(b);
		}
	};

	using fvec2 =  vec<2, float>;
	using ivec2 =  vec<2, int>;
	using uivec2 = vec<2, unsigned int>;
	using dvec2 =  vec<2, double>;
}