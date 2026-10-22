#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How the pixels of a sprite are colored when it is drawn. */
enum class SpriteTone : std::uint8_t {
  /** @brief As they are in the sheet. */
  Normal,
  /** @brief Every visible pixel in FLASH_COLOR, or brightened where the
   * graphics card has no shaders: an entity just hit. */
  Flash,
  /** @brief Every pixel multiplied by SHADE_TINT, so darker: a hit, when the
   * player reduced flashes. */
  Shade,
  /** @brief One pixel in two, in a checkerboard aligned on the canvas, the
   * others left out, or the whole sprite half transparent where the graphics
   * card has no shaders: a ship that cannot be hit. */
  Ghost,
};

}  // namespace rtype::client
