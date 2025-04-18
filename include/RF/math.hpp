#pragma once

#include <cmath>
#include "RF/maths/mat.hpp"
#include "RF/maths/vecs.hpp"
#include "RF/template.hpp"

namespace RF
{
	constexpr long double pi_value = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648;

	template<typename A, RF_arithmetic_template(A)>
	constexpr A pi(std::uint8_t digits = 16)
	{
		double scale = std::pow(10.0, digits);
		return static_cast<A>(std::round(RF::pi_value * scale) / scale);
	}

	template<typename A, typename D, RF_arithmetic_template(A), RF_arithmetic_template(D)>
	constexpr A radians(D degrees)
	{
		return static_cast<A>(degrees) * RF::pi<A>(16) / static_cast<A>(180.00);
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<3, 3, A> mat4_to_mat3(const mat<4, 4, A> &m)
	{
		mat<3, 3, A> result;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result[i][j] = m[i][j];
			}
		}

		return result;
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> transpose(const mat<4, 4, A> &m)
	{
		mat<4, 4, A> result;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result[i][j] = m[j][i];
			}
		}

		return result;
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> inverse(const mat<4, 4, A> &m)
	{
		A coef00 = m[2].z * m[3].w - m[3].z * m[2].w;
		A coef02 = m[1].z * m[3].w - m[3].z * m[1].w;
		A coef03 = m[1].z * m[2].w - m[2].z * m[1].w;

		A coef04 = m[2].y * m[3].w - m[3].y * m[2].w;
		A coef06 = m[1].y * m[3].w - m[3].y * m[1].w;
		A coef07 = m[1].y * m[2].w - m[2].y * m[1].w;

		A coef08 = m[2].y * m[3].z - m[3].y * m[2].z;
		A coef10 = m[1].y * m[3].z - m[3].y * m[1].z;
		A coef11 = m[1].y * m[2].z - m[2].y * m[1].z;

		A coef12 = m[2].x * m[3].w - m[3].x * m[2].w;
		A coef14 = m[1].x * m[3].w - m[3].x * m[1].w;
		A coef15 = m[1].x * m[2].w - m[2].x * m[1].w;

		A coef16 = m[2].x * m[3].z - m[3].x * m[2].z;
		A coef18 = m[1].x * m[3].z - m[3].x * m[1].z;
		A coef19 = m[1].x * m[2].z - m[2].x * m[1].z;

		A coef20 = m[2].x * m[3].y - m[3].x * m[2].y;
		A coef22 = m[1].x * m[3].y - m[3].x * m[1].y;
		A coef23 = m[1].x * m[2].y - m[2].x * m[1].y;

		vec<4, A> fac0(coef00, coef00, coef02, coef03);
		vec<4, A> fac1(coef04, coef04, coef06, coef07);
		vec<4, A> fac2(coef08, coef08, coef10, coef11);
		vec<4, A> fac3(coef12, coef12, coef14, coef15);
		vec<4, A> fac4(coef16, coef16, coef18, coef19);
		vec<4, A> fac5(coef20, coef20, coef22, coef23);

		vec<4, A> vec0(m[1].x, m[0].x, m[0].x, m[0].x);
		vec<4, A> vec1(m[1].y, m[0].y, m[0].y, m[0].y);
		vec<4, A> vec2(m[1].z, m[0].z, m[0].z, m[0].z);
		vec<4, A> vec3(m[1].w, m[0].w, m[0].w, m[0].w);

		vec<4, A> inv0 = vec1 * fac0 - vec2 * fac1 + vec3 * fac2;
		vec<4, A> inv1 = vec0 * fac0 - vec2 * fac3 + vec3 * fac4;
		vec<4, A> inv2 = vec0 * fac1 - vec1 * fac3 + vec3 * fac5;
		vec<4, A> inv3 = vec0 * fac2 - vec1 * fac4 + vec2 * fac5;

		vec<4, A> sign_a(+1, -1, +1, -1);
		vec<4, A> sign_b(-1, +1, -1, +1);

		mat<4, 4, A> inverse;
		inverse[0] = inv0 * sign_a;
		inverse[1] = inv1 * sign_b;
		inverse[2] = inv2 * sign_a;
		inverse[3] = inv3 * sign_b;

		A det = m[0].x * inverse[0].x + m[0].y * inverse[1].x + m[0].z * inverse[2].x + m[0].w * inverse[3].x;
		return inverse * (static_cast<A>(1) / det);
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> look_at(const vec<3, A> &eye, const vec<3, A> &center, const vec<3, A> &up)
	{
		vec<3, A> f = (center - eye).unit();
		vec<3, A> s = cross(f, up.unit()).unit();
		vec<3, A> u = cross(s, f);

		mat<4, 4, A> result;
		result[0] = vec<4, A>(s.x, u.x, -f.x, 0);
		result[1] = vec<4, A>(s.y, u.y, -f.y, 0);
		result[2] = vec<4, A>(s.z, u.z, -f.z, 0);
		result[3] = vec<4, A>(-s.template dot<A>(eye), -u.template dot<A>(eye), f.template dot<A>(eye), 1);

		return result;
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> perspective(A fov_radians, A aspect, A z_near, A z_far)
	{
		A f = static_cast<A>(1) / std::tan(fov_radians / static_cast<A>(2));
		A range_inv = static_cast<A>(1) / (z_near - z_far);

		mat<4, 4, A> result;
		result[0] = vec<4, A>(f / aspect, 0, 0, 0);
		result[1] = vec<4, A>(0, f, 0, 0);
		result[2] = vec<4, A>(0, 0, (z_far + z_near) * range_inv, -1);
		result[3] = vec<4, A>(0, 0, (static_cast<A>(2) * z_far * z_near) * range_inv, 0);
		return result;
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> orthographic(A left, A right, A bottom, A top, A z_near, A z_far)
	{
		A rl = static_cast<A>(1) / (right - left);
		A tb = static_cast<A>(1) / (top - bottom);
		A fn = static_cast<A>(1) / (z_far - z_near);

		mat<4, 4, A> result;
		result[0] = vec<4, A>(static_cast<A>(2) * rl, 0, 0, 0);
		result[1] = vec<4, A>(0, static_cast<A>(2) * tb, 0, 0);
		result[2] = vec<4, A>(0, 0, -static_cast<A>(2) * fn, 0);
		result[3] = vec<4, A>(-(right + left) * rl, -(top + bottom) * tb, -(z_far + z_near) * fn, 1);
		return result;
	}

	template<typename A, RF_arithmetic_template(A)>
	constexpr mat<4, 4, A> angle_axis(A angle_rad, const vec<3, A> &axis)
	{
		vec<3, A> a = axis.unit();
		A c = std::cos(angle_rad);
		A s = std::sin(angle_rad);
		A t = static_cast<A>(1) - c;

		mat<4, 4, A> result;

		result[0] = vec<4, A>(t * a.x * a.x + c,       t * a.x * a.y - s * a.z, t * a.x * a.z + s * a.y, 0);
		result[1] = vec<4, A>(t * a.x * a.y + s * a.z, t * a.y * a.y + c,       t * a.y * a.z - s * a.x, 0);
		result[2] = vec<4, A>(t * a.x * a.z - s * a.y, t * a.y * a.z + s * a.x, t * a.z * a.z + c,       0);
		result[3] = vec<4, A>(0, 0, 0, 1);

		return result;
	}
}
