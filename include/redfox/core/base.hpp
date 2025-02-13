#pragma once

#include "redfox/core/utils/ignoramus.hpp" // engine logging RF::ignoramus::logf

// TYPES:
#include "redfox/core/types/vector/vec2.hpp" // RF::vec<2, T> and various typedefs
#include "redfox/core/types/vector/vec3.hpp" // RF::vec<3, T> and various typedefs
#include "redfox/core/types/memory.hpp"      // RF::memory_t<scale> for networking and RF::pool<T>
#include "redfox/core/types/mass.hpp"        // RF::mass<T, n>
#include "redfox/core/types/pool.hpp"        // RF::pool<T>

#include "redfox/core/types/library.hpp"     // RF::library abstract layer for Unix dlopen and Windows LoadLibrary

// INTERFACE:
#include "redfox/core/interface/delegate.hpp" // RF::delegate
#include "redfox/core/interface/monitor.hpp"  // RF::MonitorData
#include "redfox/core/interface/window.hpp"   // RF::window