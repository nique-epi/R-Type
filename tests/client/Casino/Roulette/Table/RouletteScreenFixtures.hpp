#pragma once

#include <cstdint>
#include "RouletteRound.hpp"

namespace rtype::client::testing {

/** @returns Round @p number with its bets open for TWELVE_SECONDS_OF_TICKS,
 * ACE and LUNA seated, ACE with @p aceStake on the cloth. */
[[nodiscard]] network::RouletteRound bettingRound(std::uint16_t number,
                                                  std::uint32_t aceStake);

/** @returns A_ROUND, the wheel turning to SEVENTEEN_WINS, ACE with
 * STAKE_OF_ACE on the cloth. */
[[nodiscard]] network::RouletteRound spinningRound();

/** @returns A_ROUND at its result, SEVENTEEN_WINS drawn, ACE with
 * STAKE_OF_ACE on the cloth handed back @p acePayout. */
[[nodiscard]] network::RouletteRound resultRound(std::uint32_t acePayout);

}  // namespace rtype::client::testing
