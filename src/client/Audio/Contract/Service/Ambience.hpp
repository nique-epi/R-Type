#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The sound that plays under the theme to color a screen: nothing, deep
 * space on the server map, a garage in the hangar. */
enum class Ambience : std::uint8_t {
  None,
  Space,
  Hangar,
};

}  // namespace rtype::client
