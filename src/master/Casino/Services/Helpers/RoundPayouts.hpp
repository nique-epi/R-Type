#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include "PlayerBet.hpp"
#include "RouletteBet.hpp"

namespace rtype::master {

/** @returns The chips each account staked in @p bets. */
[[nodiscard]] std::map<std::int64_t, std::uint32_t> stakesOf(
    const std::vector<PlayerBet>& bets);

/** @returns The bets of each account in @p bets, in the order they were
 * laid. */
[[nodiscard]] std::map<std::int64_t, std::vector<network::RouletteBet>>
betsByPlayerOf(const std::vector<PlayerBet>& bets);

/** @returns What @p bets return when @p drawn comes out, by account
 * (network::payoutOf()); an account all of whose bets lost has 0. */
[[nodiscard]] std::map<std::int64_t, std::uint32_t> payoutsOf(
    const std::vector<PlayerBet>& bets, std::uint8_t drawn);

}  // namespace rtype::master
