#include "RF/interface/window.hpp"
#include "RF/interface/virtual_key.hpp"

RF::window::window(RF::window_info info) : info_(std::move(info)), state_(RF::window_state_t::Focused)
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