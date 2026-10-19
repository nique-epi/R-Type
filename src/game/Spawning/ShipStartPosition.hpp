#pragma once

#include <cstddef>
#include <cstdint>
#include "Position.hpp"

namespace rtype::game {

/**
 * @brief Where the ship of a player appears: the left of the playfield, at
 * SHIP_START_X_RATIO of its width, and on a row of its own.
 *
 * The playfield height is cut into @p playerCount + 1 equal parts and player
 * n takes the n-th cut, so the ships of two players are always
 * PLAYFIELD_HEIGHT / (playerCount + 1) apart, and none touches an edge.
 *
 * @param playerNumber 1 to @p playerCount.
 * @param playerCount The most players a game holds; at least 1.
 * @throws InvalidPlayerNumberException when @p playerNumber is 0 or above
 * @p playerCount.
 */
[[nodiscard]] Position shipStartPosition(std::uint8_t playerNumber,
                                         std::size_t playerCount);

}  // namespace rtype::game
