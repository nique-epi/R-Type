#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include "ConnectedPlayers.hpp"
#include "MapChoice.hpp"

namespace rtype::server {

/**
 * @returns The index, from 0, of the map the vote of @p players chose among
 * @p maps: the map most players who are not away vote for; between maps with
 * as many votes, one drawn with @p seed, and, when no player votes, one drawn
 * among them all. A vote is the number ConnectedPlayer::mapVote holds, from
 * 1; NO_MAP_VOTE and a number past the last map count for none.
 * @param maps One or more.
 */
[[nodiscard]] std::size_t votedMapOf(const ConnectedPlayers& players,
                                     std::span<const MapChoice> maps,
                                     std::uint32_t seed);

}  // namespace rtype::server
