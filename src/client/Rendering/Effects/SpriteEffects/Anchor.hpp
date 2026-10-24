#pragma once

#include <SFML/System/Vector2.hpp>
#include "Entity.hpp"

namespace rtype::client {

/**
 * @brief Component of an effect that follows another entity: SpriteEffectSystem
 * puts it at the Position of that entity plus `offset`, in logical units, at
 * each step, so a muzzle flash stays on the nose of a ship that moves. Once
 * the entity followed is gone, the effect stays where it was.
 */
struct Anchor {
  engine::Entity entity{};
  sf::Vector2f offset;
};

}  // namespace rtype::client
