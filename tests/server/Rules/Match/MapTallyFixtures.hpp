#pragma once

#include <cstddef>
#include <cstdint>
#include <set>
#include <span>
#include <vector>
#include "ConnectedPlayer.hpp"

namespace rtype::server::testing {

/** @returns One player per vote of @p votes, numbered from 1, none away. */
[[nodiscard]] std::vector<ConnectedPlayer> votingPlayers(
    std::span<const std::uint8_t> votes);

/** @returns The maps votedMapOf() chooses among @p mapCount for @p players,
 * over SEEDS_TRIED seeds. */
[[nodiscard]] std::set<std::size_t> mapsChosenOverSeeds(
    std::vector<ConnectedPlayer>& players, std::size_t mapCount);

/** @returns The map votedMapOf() chooses among @p mapCount for @p players,
 * with the first seed. */
[[nodiscard]] std::size_t mapChosen(std::vector<ConnectedPlayer>& players,
                                    std::size_t mapCount);

}  // namespace rtype::server::testing
