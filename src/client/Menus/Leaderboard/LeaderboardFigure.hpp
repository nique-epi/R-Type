#pragma once

#include <SFML/System/Vector2.hpp>
#include "Animation.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief A banner or a guard of the court: what it shows, where, and how far
 * ahead of the others it runs, so two of them never move together. */
struct LeaderboardFigure {
  const Animation* animation{};
  sf::Vector2i topLeft;
  engine::Duration lead{};
};

}  // namespace rtype::client
