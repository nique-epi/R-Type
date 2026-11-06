#pragma once

#include <cstddef>

namespace rtype::server {

struct ConnectedPlayer;

/**
 * @returns The place of @p player among the seats of a game, 0 for the player
 * numbered FIRST_PLAYER_NUMBER up to MAXIMUM_PLAYERS - 1, for a table a module
 * keeps per seat. The entry of a seat must be cleared when its session closes:
 * the next player on that seat is someone else.
 */
[[nodiscard]] std::size_t seatIndexOf(const ConnectedPlayer& player);

}  // namespace rtype::server
