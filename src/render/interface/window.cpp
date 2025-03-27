#include "RF/interface/window.hpp"

RF::window::window(RF::window_info info)
:
	info_(std::move(info)), state_(RF::window_state_t::Focused),
	mouse_position_(0, 0), flags_(RF::window_flag_bit_t::None)
{
	auto initialize_keys = [this](auto key_enum, auto count_elem, auto& key_states)
	{
		for (unsigned int i = 0; i < static_cast<unsigned int>(count_elem); ++i)
		{
			auto key = static_cast<typename std::decay<decltype(key_enum)>::type>(i);
			key_states[key] = RF::key_state_t::Inactive;
		}
	};

	initialize_keys(RF::virtual_key_t{}, RF::virtual_key_t::_Count, this->virtual_key_states_);
	initialize_keys(RF::mouse_key_t{}, RF::mouse_key_t::_Count, this->mouse_key_states_);
}

RF::window_info RF::window::get_info() const
{  return this->info_;  }

RF::window_state_t RF::window::get_state() const
{ return this->state_; }

#include <stdexcept>
RF::key_state_t RF::window::get_key_state(RF::virtual_key_t key) const
{
	auto it = this->virtual_key_states_.find(key);
	if (it == this->virtual_key_states_.end())
	{ throw std::runtime_error("fatal error, RF::virtual_key_t not found in key states umap"); }
	return it->second;
}

void RF::window::set_flag(RF::window_flag_bit_t flags, bool enabled)
{
	if (enabled)
	{
		this->flags_ |= flags;
	}
	else
	{
		this->flags_ &= ~flags;
	}

	this->handle_flag_update_(flags, enabled);
}

bool RF::window::get_flag(RF::window_flag_bit_t flag) const
{
	return (this->flags_ & flag) != RF::window_flag_bit_t::None;
}

RF::window_flag_bit_t RF::operator|(RF::window_flag_bit_t flag_a, RF::window_flag_bit_t flag_b)
{
	return static_cast<RF::window_flag_bit_t>(static_cast<std::uint32_t>(flag_a) | static_cast<std::uint32_t>(flag_b));
}

RF::window_flag_bit_t RF::operator&(RF::window_flag_bit_t flag_a, RF::window_flag_bit_t flag_b)
{
	return static_cast<RF::window_flag_bit_t>(static_cast<std::uint32_t>(flag_a) & static_cast<std::uint32_t>(flag_b));
}

RF::window_flag_bit_t RF::operator^(RF::window_flag_bit_t flag_a, RF::window_flag_bit_t flag_b)
{
	return static_cast<RF::window_flag_bit_t>(static_cast<std::uint32_t>(flag_a) ^ static_cast<std::uint32_t>(flag_b));
}

RF::window_flag_bit_t RF::operator~(RF::window_flag_bit_t flag)
{
	return static_cast<RF::window_flag_bit_t>(~static_cast<std::uint32_t>(flag));
}

RF::window_flag_bit_t &RF::operator|=(RF::window_flag_bit_t &flag_a, RF::window_flag_bit_t flag_b)
{
	flag_a = static_cast<RF::window_flag_bit_t>(static_cast<std::uint32_t>(flag_a) | static_cast<std::uint32_t>(flag_b));
	return flag_a;
}

RF::window_flag_bit_t &RF::operator&=(RF::window_flag_bit_t &flag_a, RF::window_flag_bit_t flag_b)
{
	flag_a = static_cast<RF::window_flag_bit_t>(static_cast<std::uint32_t>(flag_a) & static_cast<std::uint32_t>(flag_b));
	return flag_a;
}
