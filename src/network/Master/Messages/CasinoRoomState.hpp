#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CasinoPlayer.hpp"
#include "MillisecondDate.hpp"
#include "RoulettePhase.hpp"

namespace rtype::network {

/**
 * @brief The state of one roulette room: the answer of
 * `GET /api/casino/rooms/{room}`. Its dates are milliseconds since 1970 on
 * the clock of the master, which serverTime gives, so a client times the
 * phases on its own clock moved by the difference.
 */
struct CasinoRoomState {
  std::uint8_t id{};
  std::string name;
  std::uint8_t seats{};
  std::uint64_t round{};
  RoulettePhase phase{RoulettePhase::Betting};
  /** @brief When the bets of the round close and the wheel spins. */
  MillisecondDate closesAt;
  /** @brief When the wheel stops and the result shows. */
  MillisecondDate resultAt;
  /** @brief When the round ends and the next one opens its bets. */
  MillisecondDate endsAt;
  /** @brief The clock of the master when it answered. */
  MillisecondDate serverTime;
  /** @brief The number of the round, once drawn: never while the bets are
   * open. */
  std::optional<std::uint8_t> winningNumber = std::nullopt;
  /** @brief The numbers of the rounds before this one, newest first,
   * CASINO_HISTORY_LENGTH at most. */
  std::vector<std::uint8_t> history;
  /** @brief The players seated now. */
  std::vector<CasinoPlayer> players;

  bool operator==(const CasinoRoomState&) const = default;
};

}  // namespace rtype::network
