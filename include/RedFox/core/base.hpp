#pragma once

#include "RedFox/core/utils/ignoramus.hpp" // engine logging RF::ignoramus::logf

// TYPES:
#include "RedFox/core/types/vector/vec2.hpp" // RF::vec<2, T> and various typedefs
#include "RedFox/core/types/vector/vec3.hpp" // RF::vec<3, T> and various typedefs
#include "RedFox/core/types/memory.hpp"      // RF::memory_t<scale> for networking and RF::pool<T>
#include "RedFox/core/types/mass.hpp"        // RF::mass<T, n>
#include "RedFox/core/types/pool.hpp"        // RF::pool<T>

#include "RedFox/core/types/library.hpp"     // RF::library abstract layer for Unix dlopen and Windows LoadLibrary