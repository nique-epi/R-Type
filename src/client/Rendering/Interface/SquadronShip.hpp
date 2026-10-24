#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/**
 * @brief One ship of the formation: its player color, by its row in the entity
 * sheet; its place in the box, in pixels; when it enters, in seconds; the
 * phase of its bobbing, in radians; how far below its place it enters, in
 * pixels.
 */
struct SquadronShip {
  int color{};
  sf::Vector2f place;
  float arrival{};
  float bobPhase{};
  float entryDrop{};
};

}  // namespace rtype::client
