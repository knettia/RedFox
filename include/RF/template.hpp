#pragma once

#include <type_traits>

namespace RF
{
	template<typename src_type, typename... target_types>
	struct is_any_equivalent : std::disjunction<std::is_same<src_type, target_types>...> {};

	template<typename src_type, typename... target_types>
	struct is_any_convertible : std::disjunction<std::is_convertible<src_type, target_types>...> {};

	template<typename src_type, typename target_type>
	struct is_derived_from : std::conjunction<
		std::is_class<src_type>,
		std::is_class<target_type>,
		std::is_base_of<src_type, target_type>
	> {};
} // namespace RF

// macros
#define RF_equivalent_template(src_type, ...) \
	typename = std::enable_if<RF::is_any_equivalent<src_type, __VA_ARGS__>::value>

#define RF_convertible_template(src_type, ...) \
	typename = std::enable_if<RF::is_any_convertible<src_type, __VA_ARGS__>::value>

#define RF_arithmetic_template(src_type) \
	typename = std::enable_if_t<std::is_arithmetic<src_type>::value>

#define RF_derived_template(derived_type, base_type) \
	typename = std::enable_if_t<RF::is_derived_from<derived_type, base_type>::value>

#define RF_enum_template(src_type) \
	typename = std::enable_if_t<std::is_enum_v<src_type>>

#define RF_underlying_template(underlying_type, base_type) \
	typename = std::enable_if_t<std::is_same_v<std::underlying_type_t<underlying_type>, base_type>>
