#pragma once

#include <SFML/System/Vector2.hpp>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief Component of an entity drawn a few pixels away from its Position for
 * a moment: the knockback of a hit, the recoil of a shot, the vibration of a
 * victim frozen before it explodes. Its Position, the place the server gave,
 * never moves.
 *
 * RenderSystem draws it `amount` pixels of the canvas away, or minus `amount`
 * on every odd `flipEvery` step of `elapsed` when `flipEvery` is positive,
 * which makes it vibrate. SpriteEffectSystem moves `elapsed` forward and
 * removes the component once it reaches `duration`.
 */
struct Nudge {
  sf::Vector2i amount;
  engine::Duration flipEvery{};
  engine::Duration elapsed{};
  engine::Duration duration{};
};

}  // namespace rtype::client
