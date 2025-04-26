#pragma once

#include "RF/maths/mat/def.hpp"
#include "RF/maths/vec/vec3.hpp"

namespace RF
{
	template<typename A>
	struct alignas(16) mat<3, 3, A>
	{
	private:
		RF::vec<3, A> columns[3];
	public:
		constexpr mat<3, 3, A>()
		{
			for (int i = 0; i < 3; ++i)
			{
				this->columns[i] = RF::vec<3, A>();
			}
		}

		template<typename T>
		constexpr mat<3, 3, A>(mat<3, 3, T> m)
		{
			for (int i = 0; i < 3; ++i)
			{
				this->columns[i] = static_cast<RF::vec<3, A>>(m[i]);
			}
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<3, 3, A>(const T s)
		{
			this->columns[0] = RF::vec<3, A>(s, 0, 0);
			this->columns[1] = RF::vec<3, A>(0, s, 0);
			this->columns[2] = RF::vec<3, A>(0, 0, s);
		}

		template<
			typename T_x0, typename T_y0, typename T_z0,
			typename T_x1, typename T_y1, typename T_z1,
			typename T_x2, typename T_y2, typename T_z2,
			RF_arithmetic_template(T_x0), RF_arithmetic_template(T_y0), RF_arithmetic_template(T_z0),
			RF_arithmetic_template(T_x1), RF_arithmetic_template(T_y1), RF_arithmetic_template(T_z1),
			RF_arithmetic_template(T_x2), RF_arithmetic_template(T_y2), RF_arithmetic_template(T_z2)
		>
		constexpr mat<3, 3, A>(
			const T_x0 x0, const T_y0 y0, const T_z0 z0,
			const T_x1 x1, const T_y1 y1, const T_z1 z1,
			const T_x2 x2, const T_y2 y2, const T_z2 z2
		)
		{
			this->columns[0] = RF::vec<3, A>(x0, y0, z0);
			this->columns[1] = RF::vec<3, A>(x1, y1, z1);
			this->columns[2] = RF::vec<3, A>(x2, y2, z2);
		}

		template<
			typename T_v0,
			typename T_v1,
			typename T_v2
		>
		constexpr mat<3, 3, A>(
			const RF::vec<3, T_v0> &v0,
			const RF::vec<3, T_v1> &v1,
			const RF::vec<3, T_v2> &v2
		)
		{
			this->columns[0] = static_cast<RF::vec<3, A>>(v0);
			this->columns[1] = static_cast<RF::vec<3, A>>(v1);
			this->columns[2] = static_cast<RF::vec<3, A>>(v2);
		}

		constexpr mat<3, 3, A>(const RF::vec<3, A> (&init_columns)[3])
		{
			for (int i = 0; i < 3; ++i)
			{
				this->columns[i] = init_columns[i];
			}
		}

		constexpr RF::vec<3, A> &operator[](int index)
		{
			return this->columns[index];
		}

		constexpr const RF::vec<3, A> &operator[](int index) const
		{
			return this->columns[index];
		}

		template<typename T>
		constexpr mat<3, 3, A> operator+(const mat<3, 3, T> &other) const
		{
			mat<3, 3, A> result;
			for (int i = 0; i < 3; ++i)
			{
				result.columns[i] = this->columns[i] + other[i];
			}
			return result;
		}

		template<typename T>
		constexpr mat<3, 3, A> operator-(const mat<3, 3, T> &other) const
		{
			mat<3, 3, A> result;
			for (int i = 0; i < 3; ++i)
			{
				result.columns[i] = this->columns[i] - other[i];
			}
			return result;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<3, 3, A> operator*(const T &scalar) const
		{
			mat<3, 3, A> result;
			for (int i = 0; i < 3; ++i)
			{
				result.columns[i] = this->columns[i] * scalar;
			}
			return result;
		}

		template<typename T>
		constexpr mat<3, 3, A> operator*(const mat<3, 3, T> &other) const
		{
			mat<3, 3, A> result;

			for (int col = 0; col < 3; ++col)
			{
				for (int row = 0; row < 3; ++row)
				{
					A sum = A();
					for (int i = 0; i < 3; ++i)
					{
						sum += this->columns[i][row] * other[col][i];
					}
					result[col][row] = sum;
				}
			}

			return result;
		}

		template<typename T>
		constexpr vec<3, A> operator*(const vec<3, T> &v) const
		{
			vec<3, A> result;

			for (int row = 0; row < 3; ++row)
			{
				A sum = A();
				for (int col = 0; col < 3; ++col)
				{
					sum += this->columns[col][row] * v[col];
				}
				result[row] = sum;
			}

			return result;
		}
	};

	// base
	using mat3x3 =   mat<3, 3, float>;
	using fmat3x3 =  mat<3, 3, float>;
	using dmat3x3 =  mat<3, 3, double>;
	using imat3x3 =  mat<3, 3, std::int32_t>;
	using uimat3x3 = mat<3, 3, std::uint32_t>;

} // namespace RF
