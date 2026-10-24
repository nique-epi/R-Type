#pragma once

#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of a ship arriving: hidden on the first half of
 * `duration`, then shown from its top row down, row by row, until the whole
 * frame shows, behind a leading line of FLASH_COLOR when `leadingLine`.
 * RenderSystem reads it; SpriteEffectSystem moves `age` forward and removes it
 * once `duration` has passed. The ship stands at its true Position all along.
 */
struct Reveal {
  engine::Duration age{};
  engine::Duration duration{};
  bool leadingLine = true;
};

}  // namespace rtype::client
