#pragma once

#include <span>
#include <vector>

#include "RedFox/net/types/body.hpp"
#include "RF/template.hpp"

namespace RF::net
{
	class message
	{
	private:
		std::uint16_t id_;
		std::vector<RF::net::body> bodies_;
	public:
		message(std::uint16_t id) : id_(id), bodies_(0) { }

		message(std::uint16_t id, std::span<RF::net::body> bodies) : id_(id)
		{ bodies_.assign(bodies.begin(), bodies.end()); }
	
		template <typename E, RF_enum_template(E), RF_underlying_template(E, std::uint16_t)>
		message(E msg) : id_(static_cast<std::uint16_t>(msg)) { }

		template <typename E, RF_enum_template(E), RF_underlying_template(E, std::uint16_t)>
		message(E msg, std::span<RF::net::body> bodies) : id_(static_cast<std::uint16_t>(msg))
		{ bodies_.assign(bodies.begin(), bodies.end()); }
		
		~message() = default;

		std::uint16_t id() const { return this->id_; }

		template <typename E, RF_enum_template(E), RF_underlying_template(E, std::uint16_t)>
		E id() const { return static_cast<E>(this->id_); }

		std::span<const RF::net::body> bodies() const { return this->bodies_; }
	};
} // namespace RF::net