#pragma once

#include <cstdint>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"

namespace rtype::client::testing {

/** @returns The room of the tests at round @p round, its bets open. */
[[nodiscard]] network::CasinoRoomState roomAtRound(std::uint64_t round);

/** @returns The answer numbered @p ask of a master that took @p bets on
 * @p round, CHIPS_LEFT left in the wallet. */
[[nodiscard]] BetsReading takenBets(
    std::uint64_t ask, std::uint64_t round,
    const std::vector<network::RouletteBet>& bets);

/** @returns The answer numbered @p ask of a master that refused bets on
 * @p round, too few chips in the wallet. */
[[nodiscard]] BetsReading refusedBets(std::uint64_t ask, std::uint64_t round);

}  // namespace rtype::client::testing
