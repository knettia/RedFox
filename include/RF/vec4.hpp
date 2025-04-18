#pragma once

#include "RF/vec.hpp"

namespace RF
{
	template<typename A>
	struct alignas(16) vec<4, A>
	{
		A x;
		A y;
		A z;
		A w;

		constexpr vec<4, A>() : x(A()), y(A()), z(A()) { }
		constexpr vec<4, A>(A x, A y, A z) : x(x), y(y), z(z) { }

		template<typename T_x, typename T_y, typename T_z, typename T_w, RF_arithmetic_template(T_x), RF_arithmetic_template(T_y), RF_arithmetic_template(T_z), RF_arithmetic_template(T_w)>
		constexpr vec<4, A>(T_x x, T_y y, T_z z, T_w w)
		{
			this->x = static_cast<A>(x);
			this->y = static_cast<A>(y);
			this->z = static_cast<A>(z);
			this->w = static_cast<A>(w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator+(const vec<4, T> &v) const
		{
			return vec<4, A>(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator+(const T &a) const
		{
			return vec<4, A>(x + a, y + a, z + a, w + a);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator-(const vec<4, T> &v) const
		{
			return vec<4, A>(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator-(const T &a) const
		{
			return vec<4, A>(x - a, y - a, z - a, w - a);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator*(const vec<4, T> &v) const
		{
			return vec<4, A>(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator*(const T s) const
		{
			return vec<4, A>(x * s, y * s, z * s, w * s);
		}
		
		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator/(const vec<4, T> &v) const
		{
			return vec<4, A>(x / v.x, y / v.y, z / v.z, w / v.w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator/(const T s) const
		{
			return vec<4, A>(x / s, y / s, z / s, w / s);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator=(const vec<4, T> &v) const
		{
			return vec<4, A>(static_cast<A>(v.x), static_cast<A>(v.y), static_cast<A>(v.z), static_cast<A>(v.w));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<4, A> operator=(const T &a) const
		{
			return vec<4, A>(static_cast<A>(a), static_cast<A>(a), static_cast<A>(a), static_cast<A>(a));
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const vec<4, T> &v) const
		{
			return (x == v.x) && (y == v.y) && (z == v.z) && (w == v.w);
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr bool operator==(const T &a) const
		{
			return (x == a) && (y == a) && (z == a) && (w == a);
		}

		// engine needs c++20 to compile anyway
		constexpr float length() const
		{
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		// engine needs c++20 to compile anyway
		constexpr vec<4, A> unit() const
		{
			float l = length();
			return *this / l;
		}

		template<typename D, typename T, RF_arithmetic_template(D), RF_arithmetic_template(T)>
		constexpr D dot(const vec<2, T> &v) const
		{
			return static_cast<D>(x) * v.x + static_cast<D>(y) * v.y + static_cast<D>(z) * v.z + static_cast<D>(w) * v.w;
		}
	};

	template<typename A, RF_arithmetic_template(A)>
	constexpr RF::vec<4, A> cross(const RF::vec<4, A> &a, const RF::vec<4, A> &b)
	{
		return RF::vec<4, A>
		{
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		};
	}

	using fvec4 =  vec<4, float>;
	using ivec4 =  vec<4, std::int32_t>;
	using uivec4 = vec<4, std::uint32_t>;
	using dvec4 =  vec<4, double>;
}
