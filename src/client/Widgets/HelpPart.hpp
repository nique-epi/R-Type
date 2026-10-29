#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief The part a step of the help lights, the rest of the picture dimmed;
 * None dims all of it.
 *
 * The parts of the waiting room lie where the room paints them, those of a
 * game where the help paints its moment of a game.
 */
enum class HelpPart : std::uint8_t {
  None,
  Bays,
  OwnBay,
  Ready,
  Status,
  Back,
  Help,
  Ship,
  Shots,
  Bydos,
  EnemyShots,
  OwnBand,
};

}  // namespace rtype::client
