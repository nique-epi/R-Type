#pragma once

#include <cstdint>

namespace rtype::network {

/**
 * @brief Why a balance of a wallet moved, written in lowercase in JSON:
 * `opening` (the starting balance), `game` (a game won kopecks), `exchange`
 * (the cashier), `purchase` (the shop), `stake`, `refund` and `payout` (the
 * casino).
 */
enum class MovementReason : std::uint8_t {
  Opening,
  Game,
  Exchange,
  Purchase,
  Stake,
  Refund,
  Payout,
};

}  // namespace rtype::network
