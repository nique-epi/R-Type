#pragma once

#include <chrono>
#include "TimeConstants.hpp"

/** @brief Time of one frame the tests give the screens: about 60 frames per
 * second. */
constexpr rtype::engine::Duration ONE_FRAME = std::chrono::milliseconds(16);
