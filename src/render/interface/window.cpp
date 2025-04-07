#include "RF/definitions.hpp"

#if defined (__WINDOWS__)
#define NOMINMAX // prevent windows.h to include MIN and MAX macros
#endif

#include "RF/interface/window.hpp"

RF::window::window(RF::reference_ptr<RF::delegate> delegate, RF::window_info info)
:
	delegate_(delegate),
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

#include "RF/log.hpp"
#include "RF/interface/delegate.hpp"
RF::video_mode_t RF::window::find_fitting_video_mode_(RF::uivec2 extent)
{
	// Enumerate video modes and choose the best one for fullscreen
	// Based on the window's size.
	const std::vector<RF::video_mode_t> modes = this->delegate_->enumerate_video_modes();
	RF::video_mode_t best;

	std::uint32_t least_size_diff = UINT32_MAX;

	for (const RF::video_mode_t mode : modes)
	{
		const std::uint32_t height_diff = mode.extent.y - extent.y;
		const std::uint32_t width_diff = mode.extent.x - extent.x;

		if ((height_diff < least_size_diff) && (width_diff < least_size_diff))
		{
			best = mode;
			least_size_diff = std::min(height_diff, width_diff);
		}
	}

	// TODO: Initialise a warning prompt/dialogue informing the
	//       Developer/user about no video mode match, instead of
	//       Using a simple terminal output
	if (best.extent != extent)
	{
		RF::logf::warn(
			"Could not find video mode with extent that matches the window resolution '<0>x<1>' in the main display. Best match: '<2>x<3>'",
			extent.x,
			extent.y,
			best.extent.x,
			best.extent.y
		);
	}

	return best;
}

#include <RF/exception.hpp>
RF::key_state_t RF::window::get_key_state(RF::virtual_key_t key) const
{
	auto it = this->virtual_key_states_.find(key);
	if (it == this->virtual_key_states_.end())
	{ throw RF::engine_error("Internal inconsistency: RF::virtual_key_t '<0>' not found in window's key states umap", static_cast<std::uint32_t>(key)); }
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
