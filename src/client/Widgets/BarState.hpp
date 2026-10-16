#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How a bar or a row looks: at rest, selected, pressed, or
 * disabled. */
enum class BarState : std::uint8_t {
  Idle,
  Selected,
  Pressed,
  Disabled,
};

}  // namespace rtype::client
