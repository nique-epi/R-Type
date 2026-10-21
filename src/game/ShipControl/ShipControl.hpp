#pragma once

#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief The directions a player holds for its ship. Holding two opposite
 * directions is holding neither.
 */
struct ShipDirections {
  bool up{false};
  bool down{false};
  bool left{false};
  bool right{false};
};

/**
 * @brief Where a ship centered on @p from is after @p elapsed, its player
 * holding @p held.
 *
 * The ship moves at SHIP_SPEED towards the directions held, as fast along a
 * diagonal as along a straight line, and stops where its side meets an edge of
 * the play area, the playfield less the status band at its bottom
 * (PLAY_AREA_HEIGHT), so the whole ship stays in sight. A ship inside the play
 * area that holds nothing, or only opposite directions, does not move. The
 * position returned is always inside, even for a ship that started outside:
 * where the ship would go is moved the least needed for the whole ship to be
 * inside.
 *
 * The server runs it for each ship at every tick of a game in progress. The
 * client is meant to run it to predict its own ship, and does not yet.
 *
 * @param elapsed Zero or more.
 */
[[nodiscard]] Position shipPositionAfter(Position from, ShipDirections held,
                                         engine::Duration elapsed);

}  // namespace rtype::game
