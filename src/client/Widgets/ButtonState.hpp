#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How a bar looks: at rest, selected, pressed, or disabled. */
enum class ButtonState : std::uint8_t {
  Idle,
  Selected,
  Pressed,
  Disabled,
};

}  // namespace rtype::client
