#pragma once

#include "RF/maths/mat/mat4x4.hpp"
#include "RF/maths/mat/mat3x3.hpp"
#include "RF/maths/vec/vec4.hpp"

namespace RF
{
	struct quaternion : RF::vec<4, float>
	{
		using vec4 = RF::vec<4, float>;

		constexpr quaternion(const float x, const float y, const float z, const float w)
		:
			RF::quaternion::vec4(x, y, z, w)
		{}

		template<typename A, RF_arithmetic_template(A)>
		constexpr quaternion(const A a)
		:
			RF::quaternion::vec4(static_cast<float>(a), static_cast<float>(a), static_cast<float>(a), static_cast<float>(a))
		{}

		template<typename A, RF_arithmetic_template(A)>
		constexpr quaternion(const vec<4, A> &v)
		:
			RF::quaternion::vec4(static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z), static_cast<float>(v.w))
		{}

		template<typename R, RF_arithmetic_template(R), typename A, RF_arithmetic_template(A)>
		void rotate(R radians, RF::vec<3, A> axis)
		{
			float angle = static_cast<float>(radians);

			RF::vec<3, A> norm = axis.unit();

			float half_angle = angle * 0.5f;
			float s = std::sin(half_angle);
			float c = std::cos(half_angle);

			RF::quaternion delta
			{
				static_cast<float>(norm.x) * s,
				static_cast<float>(norm.y) * s,
				static_cast<float>(norm.z) * s,
				c
			};

			*this = delta * *this;
		}

		inline RF::quaternion operator*(const RF::quaternion &rhs) const
		{
			return RF::quaternion
			{
				w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
				w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
				w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
				w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
			};
		}

		inline RF::mat3x3 to_mat3() const
		{
			float x2 = x + x;
			float y2 = y + y;
			float z2 = z + z;

			float xx = x * x2;
			float yy = y * y2;
			float zz = z * z2;
			float xy = x * y2;
			float xz = x * z2;
			float yz = y * z2;
			float wx = w * x2;
			float wy = w * y2;
			float wz = w * z2;

			return RF::mat3x3
			{
				{
					{ 1.0f - (yy + zz), xy + wz, xz - wy },
					{ xy - wz, 1.0f - (xx + zz), yz + wx },
					{ xz + wy, yz - wx, 1.0f - (xx + yy) }
				}
			};
		}

		inline RF::mat4x4 to_mat4() const
		{
			RF::mat3x3 rot3 = to_mat3();

			return RF::mat4x4
			{
				{
					{ rot3[0][0], rot3[0][1], rot3[0][2], 0.0f },
					{ rot3[1][0], rot3[1][1], rot3[1][2], 0.0f },
					{ rot3[2][0], rot3[2][1], rot3[2][2], 0.0f },
					{ 0.0f,       0.0f,       0.0f,       1.0f }
				}
			};
		}
	};
}