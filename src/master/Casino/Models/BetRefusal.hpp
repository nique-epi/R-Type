#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief Why new bets of the casino were refused, nothing written. */
enum class BetRefusal : std::uint8_t {
  /** @brief The round is not the one whose bets are open now. */
  BetsClosed,
  /** @brief The account holds no seat in the room. */
  NotSeated,
  /** @brief A bet lies off the cloth, or holds no chip. */
  InvalidBet,
  /** @brief More than network::MAXIMUM_ROULETTE_BETS bets. */
  TooManyBets,
  /** @brief The bets hold more than network::MAXIMUM_ROUND_STAKE chips, or
   * could return more than network::MAXIMUM_ROUND_PAYOUT. */
  OverTheCap,
  /** @brief The wallet holds fewer chips than the bets, once the bets they
   * replace are handed back. */
  NotEnoughChips,
};

}  // namespace rtype::master
