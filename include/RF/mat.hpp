#pragma once

#include "RF/vec.hpp"

// THIS MAT IS: W.I.P.
namespace RF
{
	namespace detail
	{
		template<int v>
		struct mat_guard
		{
			static constexpr bool valid = (v >= 2 && v <= 4);
		};
	} // namespace detail

	template<int n, int m, typename A, RF_arithmetic_template(A)>
	requires(detail::mat_guard<n>::valid && detail::mat_guard<m>::valid)
	struct mat;

	template<int n, int m, typename A>
	struct alignas(16) mat<n, m, A>
	{
	private:
		vec<m, A> columns[n];
	public:
		constexpr mat<n, m, A>()
		{
			for (int i = 0; i < n; ++i)
			{
				columns[i] = vec<m, A>();
			}
		}

		constexpr mat<n, m, A>(const vec<m, A> (&init_columns)[n])
		{
			for (int i = 0; i < n; ++i)
			{
				columns[i] = init_columns[i];
			}
		}

		constexpr vec<m, A> &operator[](int index)
		{
			return columns[index];
		}

		constexpr const vec<m, A> &operator[](int index) const
		{
			return columns[index];
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<n, m, A> operator+(const mat<n, m, T> &other) const
		{
			mat<n, m, A> result;
			for (int i = 0; i < n; ++i)
			{
				result.columns[i] = this->columns[i] + other[i];
			}
			return result;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<n, m, A> operator-(const mat<n, m, T> &other) const
		{
			mat<n, m, A> result;
			for (int i = 0; i < n; ++i)
			{
				result.columns[i] = this->columns[i] - other[i];
			}
			return result;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr mat<n, m, A> operator*(const T &scalar) const
		{
			mat<n, m, A> result;
			for (int i = 0; i < n; ++i)
			{
				result.columns[i] = this->columns[i] * scalar;
			}
			return result;
		}

		template<int k, typename T, RF_arithmetic_template(T)>
		constexpr mat<k, m, A> operator*(const mat<k, n, T> &other) const
		{
			mat<k, m, A> result;

			for (int col = 0; col < k; ++col)
			{
				for (int row = 0; row < m; ++row)
				{
					A sum = A();
					for (int i = 0; i < n; ++i)
					{
						sum += this->columns[i][row] * other[col][i];
					}
					result[col][row] = sum;
				}
			}

			return result;
		}

		template<typename T, RF_arithmetic_template(T)>
		constexpr vec<m, A> operator*(const vec<n, T> &v) const
		{
			vec<m, A> result;

			for (int row = 0; row < m; ++row)
			{
				A sum = A();
				for (int col = 0; col < n; ++col)
				{
					sum += this->columns[col][row] * v[col];
				}
				result[row] = sum;
			}

			return result;
		}
	};

	// float
	using fmat2 = mat<2, 2, float>;
	using fmat2x2 = mat<2, 2, float>;
	using fmat2x3 = mat<2, 3, float>;
	using fmat2x4 = mat<2, 4, float>;

	using fmat3 = mat<3, 3, float>;
	using fmat3x2 = mat<3, 2, float>;
	using fmat3x3 = mat<3, 3, float>;
	using fmat3x4 = mat<3, 4, float>;
	
	using fmat4 = mat<4, 4, float>;
	using fmat4x2 = mat<4, 2, float>;
	using fmat4x3 = mat<4, 3, float>;
	using fmat4x4 = mat<4, 4, float>;

	// double
	using dmat2 = mat<2, 2, double>;
	using dmat2x2 = mat<2, 2, double>;
	using dmat2x3 = mat<2, 3, double>;
	using dmat2x4 = mat<2, 4, double>;
	
	using dmat3 = mat<3, 3, double>;
	using dmat3x2 = mat<3, 2, double>;
	using dmat3x3 = mat<3, 3, double>;
	using dmat3x4 = mat<3, 4, double>;

	using dmat4 = mat<4, 4, double>;
	using dmat4x2 = mat<4, 2, double>;
	using dmat4x3 = mat<4, 3, double>;
	using dmat4x4 = mat<4, 4, double>;

	// int
	using imat2 = mat<2, 2, std::int32_t>;
	using imat2x2 = mat<2, 2, std::int32_t>;
	using imat2x3 = mat<2, 3, std::int32_t>;
	using imat2x4 = mat<2, 4, std::int32_t>;

	using imat3 = mat<3, 3, std::int32_t>;
	using imat3x2 = mat<3, 2, std::int32_t>;
	using imat3x3 = mat<3, 3, std::int32_t>;
	using imat3x4 = mat<3, 4, std::int32_t>;

	using imat4 = mat<4, 4, std::int32_t>;
	using imat4x2 = mat<4, 2, std::int32_t>;
	using imat4x3 = mat<4, 3, std::int32_t>;
	using imat4x4 = mat<4, 4, std::int32_t>;

	// unsigned int
	using uimat2 = mat<2, 2, std::uint32_t>;
	using uimat2x2 = mat<2, 2, std::uint32_t>;
	using uimat2x3 = mat<2, 3, std::uint32_t>;
	using uimat2x4 = mat<2, 4, std::uint32_t>;

	using uimat3 = mat<3, 3, std::uint32_t>;
	using uimat3x2 = mat<3, 2, std::uint32_t>;
	using uimat3x3 = mat<3, 3, std::uint32_t>;
	using uimat3x4 = mat<3, 4, std::uint32_t>;

	using uimat4 = mat<4, 4, std::uint32_t>;
	using uimat4x2 = mat<4, 2, std::uint32_t>;
	using uimat4x3 = mat<4, 3, std::uint32_t>;
	using uimat4x4 = mat<4, 4, std::uint32_t>;
} // namespace RF
