#pragma once

#include <optional>
#include <vector>
#include "ChipMoves.hpp"
#include "RouletteBet.hpp"
#include "RouletteRound.hpp"
#include "TablePlace.hpp"

namespace rtype::client {

/**
 * @brief The roulette table of the waiting room, as the screens read and
 * drive it.
 */
class ICasinoChannel {
 public:
  virtual ~ICasinoChannel() = default;

  /** @brief Asks the server to seat the player at the table, or to take it
   * back to the room: a value, sent again until the server shows it. Ignored
   * without a session or once the game started. */
  virtual void chooseTable(network::TablePlace place) = 0;

  /** @brief Lays @p bets on the cloth for the round in progress, with
   * @p spinVote, in place of the bets laid before: sent at once, then again
   * while the bets are open. Ignored unless the player sits at the table and
   * the bets are open. */
  virtual void placeBets(std::vector<network::RouletteBet> bets,
                         bool spinVote) = 0;

  /** @returns The newest state of the table the server sent; none before the
   * first one, without a session, or once the game started. */
  [[nodiscard]] virtual std::optional<network::RouletteRound> round() const = 0;

  /** @returns The chips the rounds moved since the last call, and forgets
   * them: each round counts once, when its number is first seen drawn. */
  [[nodiscard]] virtual ChipMoves takeChipMoves() = 0;
};

}  // namespace rtype::client
