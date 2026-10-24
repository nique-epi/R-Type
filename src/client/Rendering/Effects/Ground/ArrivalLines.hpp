#pragma once

#include <SFML/Graphics/Color.hpp>
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/** @brief The speed lines of a ship arriving: where it stands, the light
 * shade of its pilot, whether every other line is FLASH_COLOR, how long
 * they wait, and how long each then lives. */
struct ArrivalLines {
  game::Position shipCenter{};
  sf::Color light;
  bool flashes = true;
  engine::Duration delay{};
  engine::Duration life{};
};

}  // namespace rtype::client
