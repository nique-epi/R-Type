#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief How the window shows on the desktop: a window with a title bar, at a
 * size the player picks, or a window without border that covers the whole
 * screen, at the size of the desktop.
 */
enum class DisplayMode : std::uint8_t {
  Windowed,
  Borderless,
};

}  // namespace rtype::client
