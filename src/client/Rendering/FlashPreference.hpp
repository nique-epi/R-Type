#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Whether effects may flash, or the player turned REDUCE FLASHES on
 * and asked for no flash, blink or shake. */
enum class FlashPreference : std::uint8_t {
  Full,
  Reduced,
};

}  // namespace rtype::client
