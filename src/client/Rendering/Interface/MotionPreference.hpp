#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Whether the interface may move, or the player asked the system for
 * less motion. */
enum class MotionPreference : std::uint8_t {
  Full,
  Reduced,
};

}  // namespace rtype::client
