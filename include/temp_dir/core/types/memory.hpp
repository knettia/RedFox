#pragma once

#include <cmath>
#include <cstdint>

namespace RF
{
	using ld  = long double;
	using ull = unsigned long long;

	template <RF::ull scale_T>
	class memory_t
	{
	public:
		static constexpr RF::ull scale = scale_T;

		explicit constexpr memory_t(RF::ld count) : count_(count) { }

		constexpr RF::ld count() const
		{ return count_; }

		constexpr RF::ull bytes() const
		{ return static_cast<RF::ull>(count_ * scale); }

		constexpr bool operator<(const memory_t other) const
		{ return bytes() < other.bytes(); }

		constexpr bool operator>(const memory_t other) const
		{ return bytes() > other.bytes(); }

		constexpr bool operator<=(const memory_t other) const
		{ return bytes() <= other.bytes(); }

		constexpr bool operator>=(const memory_t other) const
		{ return bytes() >= other.bytes(); }

		constexpr bool operator==(const memory_t other) const
		{ return bytes() == other.bytes(); }

		constexpr bool operator!=(const memory_t other) const
		{ return bytes() != other.bytes(); }
	private:
		RF::ld count_;
	};

	constexpr RF::ull byte_scale     = 1;

	// 10^2
	constexpr RF::ull kilobyte_scale = 1000;
	constexpr RF::ull megabyte_scale = kilobyte_scale * 1000;
	constexpr RF::ull gigabyte_scale = megabyte_scale * 1000;
	constexpr RF::ull terabyte_scale = gigabyte_scale * 1000;
	constexpr RF::ull petabyte_scale = terabyte_scale * 1000;

	// 2^n
	constexpr RF::ull kibibyte_scale = 1ULL << 10;
	constexpr RF::ull mebibyte_scale = 1ULL << 20;
	constexpr RF::ull gibibyte_scale = 1ULL << 30;
	constexpr RF::ull tebibyte_scale = 1ULL << 40;
	constexpr RF::ull pebibyte_scale = 1ULL << 50;

	using bytes     = RF::memory_t<byte_scale>;
	using kilobytes = RF::memory_t<kilobyte_scale>;
	using megabytes = RF::memory_t<megabyte_scale>;
	using gigabytes = RF::memory_t<gigabyte_scale>;
	using terabytes = RF::memory_t<terabyte_scale>;
	using petabytes = RF::memory_t<petabyte_scale>;

	using kibibytes = RF::memory_t<kibibyte_scale>;
	using mebibytes = RF::memory_t<mebibyte_scale>;
	using gibibytes = RF::memory_t<gibibyte_scale>;
	using tebibytes = RF::memory_t<tebibyte_scale>;
	using pebibytes = RF::memory_t<pebibyte_scale>;
	
	template<RF::ull scale>
	constexpr RF::ull membytes(const RF::memory_t<scale> m)
	{ return static_cast<RF::ull>(m.count() * scale); }

	template<RF::ull to_scale, RF::ull from_scale>
	constexpr RF::memory_t<to_scale> memory_cast(const RF::memory_t<from_scale> value)
	{
		RF::ld target_count = static_cast<RF::ld>(RF::membytes(value)) / to_scale;
		return RF::memory_t<to_scale>(target_count);
	}
} // namespace RF