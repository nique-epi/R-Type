#pragma once

#include "SpriteTone.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity just hit: the tone it is drawn with, and for
 * how much longer.
 *
 * RenderSystem draws the entity with this tone; HitFlashSystem counts
 * `remaining` down and removes the component once it is spent.
 */
struct HitFlash {
  SpriteTone tone{};
  engine::Duration remaining{};
};

}  // namespace rtype::client
