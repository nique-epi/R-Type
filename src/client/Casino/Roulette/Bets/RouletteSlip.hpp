#pragma once

#include <cstdint>
#include <vector>
#include "RouletteBets.hpp"

namespace rtype::client {

/**
 * @brief The bets the player holds on the cloth for one round, one stack per
 * place, as the client sends them whole (ICasinoChannel::placeBets()).
 *
 * It takes no bet the server would refuse for its size: the stake never
 * passes MAXIMUM_ROUND_STAKE nor the chips the player holds, and the stacks
 * never pass MAXIMUM_ROULETTE_BETS.
 */
class RouletteSlip {
 public:
  /** @returns The stacks, in the order they were first laid. */
  [[nodiscard]] const std::vector<network::RouletteBet>& bets() const;

  /** @returns The chips of every stack added up. */
  [[nodiscard]] std::uint32_t stake() const;

  /**
   * @brief Adds @p bet to the stack of its place, or opens a stack there.
   * @param chipsHeld The chips the player holds, the stake laid before
   * included.
   * @returns Whether it was laid: not when the stake would pass @p chipsHeld
   * or MAXIMUM_ROUND_STAKE, nor when a new stack would pass
   * MAXIMUM_ROULETTE_BETS; the slip is unchanged then.
   */
  bool lay(const network::RouletteBet& bet, std::uint32_t chipsHeld);

  /** @brief Takes every stack off the cloth. */
  void clear();

  /**
   * @brief Lays @p bets in place of the stacks held, when they fit as lay()
   * would take them, @p chipsHeld included.
   * @returns Whether they were laid; the slip is unchanged otherwise.
   */
  bool replaceWith(const std::vector<network::RouletteBet>& bets,
                   std::uint32_t chipsHeld);

 private:
  std::vector<network::RouletteBet> bets_;
};

}  // namespace rtype::client
