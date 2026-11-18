#pragma once

#include <cstdint>
#include <optional>
#include "BetsReceipt.hpp"
#include "CasinoRefusal.hpp"
#include "RoomRound.hpp"

namespace rtype::client {

/** @brief What the master answered to bets laid on a round: its receipt, the
 * bets on the cloth and the chips left, once it took them; why not
 * otherwise, the bets it held before staying on the cloth. */
struct BetsReading {
  /** @brief The number of the ask it answers (see CasinoAsks). */
  std::uint64_t ask{};
  network::RoomRound round;
  std::optional<network::BetsReceipt> receipt;
  std::optional<CasinoRefusal> refusal;
};

}  // namespace rtype::client
