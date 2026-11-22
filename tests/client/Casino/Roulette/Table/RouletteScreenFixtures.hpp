#pragma once

#include <cstdint>
#include <vector>
#include "BetsReading.hpp"
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

namespace rtype::client::testing {

/** @returns NEBULA_LOUNGE at round @p round, its bets open for twelve
 * seconds on the clock of the master, ACE and LUNA seated, no number drawn
 * before. */
[[nodiscard]] network::CasinoRoomState bettingRoom(std::uint64_t round);

/** @returns A_ROUND a second after its bets closed, the wheel turning to
 * SEVENTEEN_WINS, five seconds before it stops, ACE with STAKE_OF_ACE on the
 * cloth. */
[[nodiscard]] network::CasinoRoomState spinningRoom();

/** @returns spinningRoom() as the master sends it once it drew the number,
 * the wheel still turning: ACE with STAKE_OF_ACE on the cloth already paid
 * @p acePayout. */
[[nodiscard]] network::CasinoRoomState drawnSpinningRoom(
    std::uint32_t acePayout);

/** @returns A_ROUND at its result, SEVENTEEN_WINS drawn, ACE with
 * STAKE_OF_ACE on the cloth handed back @p acePayout. */
[[nodiscard]] network::CasinoRoomState resultRoom(std::uint32_t acePayout);

/** @returns The bets of ACE on A_ROUND, STAKE_OF_ACE on 17, and what they
 * paid, @p payout. */
[[nodiscard]] network::RoundBets betsOfAce(std::uint32_t payout);

/** @returns The answer of a master that took @p bets on A_ROUND, the wallet
 * holding CHIPS_LEFT_TO_ACE once staked. */
[[nodiscard]] BetsReading betsTaken(
    const std::vector<network::RouletteBet>& bets);

/** @returns The answer of a master that refused bets on A_ROUND for
 * @p refusal. */
[[nodiscard]] BetsReading betsRefused(CasinoRefusal refusal);

}  // namespace rtype::client::testing
