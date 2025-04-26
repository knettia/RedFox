#pragma once

#include "RF/maths/mat/def.hpp"
#include "RF/maths/vec/vec4.hpp"

namespace RF
{
	template<typename A>
	struct alignas(16) mat<4, 4, A>
	{
	private:
		RF::vec<4, A> columns[4];
	public:
		constexpr mat<4, 4, A>()
		{
			for (int i = 0; i < 4; ++i)
			{
				this->columns[i] = RF::vec<4, A>();
			}
		}

		template<typename T>
		constexpr mat<4, 4, A>(mat<4, 4, T> m)
		{
			for (int i = 0; i < 4; ++i)
			{
				this->columns[i] = static_cast<RF::vec<4, A>>(m[i]);
			}
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<4, 4, A>(const T s)
		{
			this->columns[0] = RF::vec<4, A>(s, 0, 0, 0);
			this->columns[1] = RF::vec<4, A>(0, s, 0, 0);
			this->columns[2] = RF::vec<4, A>(0, 0, s, 0);
			this->columns[3] = RF::vec<4, A>(0, 0, 0, s);
		}

		template<
			typename T_x0, typename T_y0, typename T_z0, typename T_w0,
			typename T_x1, typename T_y1, typename T_z1, typename T_w1,
			typename T_x2, typename T_y2, typename T_z2, typename T_w2,
			typename T_x3, typename T_y3, typename T_z3, typename T_w3,
			RF_arithmetic_template(T_x0), RF_arithmetic_template(T_y0), RF_arithmetic_template(T_z0), RF_arithmetic_template(T_w0),
			RF_arithmetic_template(T_x1), RF_arithmetic_template(T_y1), RF_arithmetic_template(T_z1), RF_arithmetic_template(T_w1),
			RF_arithmetic_template(T_x2), RF_arithmetic_template(T_y2), RF_arithmetic_template(T_z2), RF_arithmetic_template(T_w2),
			RF_arithmetic_template(T_x3), RF_arithmetic_template(T_y3), RF_arithmetic_template(T_z3), RF_arithmetic_template(T_w3)
		>
		constexpr mat<4, 4, A>(
			const T_x0 x0, const T_y0 y0, const T_z0 z0, const T_w0 w0,
			const T_x1 x1, const T_y1 y1, const T_z1 z1, const T_w1 w1,
			const T_x2 x2, const T_y2 y2, const T_z2 z2, const T_w2 w2,
			const T_x3 x3, const T_y3 y3, const T_z3 z3, const T_w3 w3
		)
		{
			this->columns[0] = RF::vec<4, A>(x0, y0, z0, w0);
			this->columns[1] = RF::vec<4, A>(x1, y1, z1, w1);
			this->columns[2] = RF::vec<4, A>(x2, y2, z2, w2);
			this->columns[3] = RF::vec<4, A>(x3, y3, z3, w3);
		}

		template<
			typename T_v0,
			typename T_v1,
			typename T_v2,
			typename T_v3
		>
		constexpr mat<4, 4, A>(
			const RF::vec<4, T_v0> &v0,
			const RF::vec<4, T_v1> &v1,
			const RF::vec<4, T_v2> &v2,
			const RF::vec<4, T_v3> &v3
		)
		{
			this->columns[0] = static_cast<RF::vec<4, A>>(v0);
			this->columns[1] = static_cast<RF::vec<4, A>>(v1);
			this->columns[2] = static_cast<RF::vec<4, A>>(v2);
			this->columns[3] = static_cast<RF::vec<4, A>>(v3);
		}

		constexpr mat<4, 4, A>(const RF::vec<4, A> (&init_columns)[4])
		{
			for (int i = 0; i < 4; ++i)
			{
				this->columns[i] = init_columns[i];
			}
		}

		constexpr RF::vec<4, A> &operator[](int index)
		{
			return this->columns[index];
		}

		constexpr const RF::vec<4, A> &operator[](int index) const
		{
			return this->columns[index];
		}

		template<typename T>
		constexpr mat<4, 4, A> operator+(const mat<4, 4, T> &other) const
		{
			mat<4, 4, A> result;
			for (int i = 0; i < 4; ++i)
			{
				result.columns[i] = this->columns[i] + other[i];
			}
			return result;
		}

		template<typename T>
		constexpr mat<4, 4, A> operator-(const mat<4, 4, T> &other) const
		{
			mat<4, 4, A> result;
			for (int i = 0; i < 4; ++i)
			{
				result.columns[i] = this->columns[i] - other[i];
			}
			return result;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<4, 4, A> operator*(const T &scalar) const
		{
			mat<4, 4, A> result;
			for (int i = 0; i < 4; ++i)
			{
				result.columns[i] = this->columns[i] * scalar;
			}
			return result;
		}

		template<typename T>
		constexpr mat<4, 4, A> operator*(const mat<4, 4, T> &other) const
		{
			mat<4, 4, A> result;

			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					A sum = A();
					for (int i = 0; i < 4; ++i)
					{
						sum += this->columns[i][row] * other[col][i];
					}
					result[col][row] = sum;
				}
			}

			return result;
		}

		template<typename T>
		constexpr vec<4, A> operator*(const vec<4, T> &v) const
		{
			vec<4, A> result;

			for (int row = 0; row < 4; ++row)
			{
				A sum = A();
				for (int col = 0; col < 4; ++col)
				{
					sum += this->columns[col][row] * v[col];
				}
				result[row] = sum;
			}

			return result;
		}
	};

	// base
	using mat4x4 =   mat<4, 4, float>;
	using fmat4x4 =  mat<4, 4, float>;
	using dmat4x4 =  mat<4, 4, double>;
	using imat4x4 =  mat<4, 4, std::int32_t>;
	using uimat4x4 = mat<4, 4, std::uint32_t>;
} // namespace RF
