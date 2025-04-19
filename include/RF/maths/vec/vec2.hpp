#pragma once

#include "RF/maths/vec.hpp"

namespace RF
{
	template<typename A>
	struct alignas(8) vec<2, A>
	{
		A x;
		A y;

		constexpr vec<2, A>() : x(A()), y(A()) { }
		constexpr vec<2, A>(A x, A y) : x(x), y(y) { }

		template<typename T_x, typename T_y, RF_arithmetic_template(T_x), RF_arithmetic_template(T_y)>
		constexpr vec<2, A>(T_x x, T_y y)
		{
			this->x = static_cast<A>(x);
        		this->y = static_cast<A>(y);
		}

		constexpr A &operator[](std::uint32_t index)
		{
			switch (index)
			{
				case (0): { return x; }
				case (1): { return y; }
				default: { throw RF::runtime_error("vec\\<2, A\\>[<0>] is not a valid access because it is out of bounds.", index); }
			}
		}

		constexpr const A &operator[](std::uint32_t index) const
		{
			switch (index)
			{
				case (0): { return x; }
				case (1): { return y; }
				default: { throw RF::runtime_error("vec\\<2, A\\>[<0>] is not a valid access because it is out of bounds.", index); }
			}
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator+(const vec<2, T> &v) const
		{
			return vec<2, A>(x + v.x, y + v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator+(const T &a) const
		{
			return vec<2, A>(x + a, y + a);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator-(const vec<2, T> &v) const
		{
			return vec<2, A>(x - v.x, y - v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator-(const T &a) const
		{
			return vec<2, A>(x - a, y - a);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator*(const vec<2, T> &v) const
		{
			return vec<2, A>(x * v.x, y * v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator*(const T s) const
		{
			return vec<2, A>(x * s, y * s);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator/(const vec<2, T> &v) const
		{
			return vec<2, A>(x / v.x, y / v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator=(const vec<2, T> &v) const
		{
			return vec<2, A>(static_cast<A>(v.x), static_cast<A>(v.y));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator=(const T &a) const
		{
			return vec<2, A>(static_cast<A>(a), static_cast<A>(a));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const vec<2, T> &v) const
		{
			return (x == v.x) && (y == v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const T &a) const
		{
			return (x == a) && (y == a);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> operator/(const T s) const
		{
			return vec<2, A>(x / s, y / s);
		}
		
		// engine needs c++20 to compile anyway
		constexpr float length() const
		{
			return std::sqrt(x * x + y * y);
		}
		
		// engine needs c++20 to compile anyway
		constexpr vec<2, A> unit() const
		{
			float l = length();
			return *this / l;
		}

		template<typename D, typename T, RF_arithmetic_template(D), RF_arithmetic_template(T)>
		constexpr D dot(const vec<2, T> &v) const
		{
			return static_cast<D>(x) * v.x + static_cast<D>(y) * v.y;
		}

		constexpr vec<2, A> perp() const
		{
			return vec<2, A>(-y, x);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> hadamard(const vec<2, T> &v) const
		{
			return vec<2, A>(x * v.x, y * v.y);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> project(const vec<2, T> &b) const
		{
			A b_len_sq = b.template dot<A>(b);
			if (b_len_sq == 0) return vec<2, A>(0, 0);
			A scale = this->dot<A>(b) / b_len_sq;
			return vec<2, A>(b.x, b.y) * scale;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<2, A> reject(const vec<2, T> &b) const
		{
			return *this - this->project(b);
		}
	};

	using fvec2  = vec<2, float>;
	using ivec2  = vec<2, std::int32_t>;
	using dvec2  = vec<2, double>;
	using uivec2 = vec<2, std::uint32_t>;
}
