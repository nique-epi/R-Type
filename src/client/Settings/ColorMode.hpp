#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @brief The palette of the ships and missiles: the game's own, or one made
 * for a form of color blindness. */
enum class ColorMode : std::uint8_t {
  Off,
  Protanopia,
  Deuteranopia,
  Tritanopia,
};

/** @brief Every mode, in the order the options list them. */
constexpr std::array COLOR_MODES{ColorMode::Off, ColorMode::Protanopia,
                                 ColorMode::Deuteranopia,
                                 ColorMode::Tritanopia};

constexpr std::size_t COLOR_MODE_COUNT = COLOR_MODES.size();

}  // namespace rtype::client
