#pragma once

#include <cstddef>
#include <cstdint>
#include "ConnectedPlayers.hpp"

namespace rtype::server {

/**
 * @returns The index, from 0, of the map the vote of @p players chose among
 * @p mapCount maps: the map most players who are not away vote for; between
 * maps with as many votes, one drawn with @p seed, and, when no player votes,
 * one drawn among them all. A vote is the number ConnectedPlayer::mapVote
 * holds, from 1; NO_MAP_VOTE and a number past @p mapCount count for none.
 * @param mapCount One or more.
 */
[[nodiscard]] std::size_t votedMapOf(const ConnectedPlayers& players,
                                     std::size_t mapCount, std::uint32_t seed);

}  // namespace rtype::server
