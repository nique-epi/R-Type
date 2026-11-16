#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "RouletteBet.hpp"
#include "RouletteRound.hpp"
#include "RouletteSlip.hpp"

namespace rtype::client {

/**
 * @brief What the client keeps of the roulette table while the session
 * lasts, whether its screen shows or not: the bets of the round in progress
 * and whether the player asked to spin it, the chips the player held once
 * they are laid, the bets of the round before, and the numbers drawn.
 *
 * follow() reads each state of the table: a round of another number puts the
 * bets held aside as those of the round before, and opens an empty slip; a
 * round seen at its result adds its number to the numbers drawn, once. No
 * table, without a session or once the game started, drops the bets of the
 * round and the vote.
 */
class RouletteMemory {
 public:
  /** @brief Reads @p round, the newest state of the table, the player
   * holding @p chipsHeld chips, the stake of the slip included. */
  void follow(const std::optional<network::RouletteRound>& round,
              std::uint32_t chipsHeld);

  [[nodiscard]] RouletteSlip& slip();
  [[nodiscard]] const RouletteSlip& slip() const;

  /** @returns The bets of the last round the player laid any on. */
  [[nodiscard]] const std::vector<network::RouletteBet>& previousBets() const;

  /** @returns Whether the player asked to spin the round in progress. */
  [[nodiscard]] bool spinAsked() const;
  void askSpin();

  /** @returns The numbers drawn, the newest first, LAST_NUMBERS_KEPT at
   * most. */
  [[nodiscard]] const std::vector<std::uint8_t>& lastNumbers() const;

  /** @returns The chips the player held, its stake laid, the last time
   * follow() saw the bets open; none before. The settings show what the round
   * settled as soon as the wheel turns: the table shows these until the
   * result. */
  [[nodiscard]] std::optional<std::uint32_t> chipsBeforeDraw() const;

 private:
  /** @brief Puts the bets held aside and opens the slip of @p number. */
  void startRound(std::uint16_t number);
  /** @brief Adds the number @p round drew to the numbers drawn, once. */
  void record(const network::RouletteRound& round);

  std::optional<std::uint16_t> round_;
  RouletteSlip slip_;
  std::vector<network::RouletteBet> previousBets_;
  bool spinAsked_ = false;
  std::optional<std::uint16_t> recordedRound_;
  std::vector<std::uint8_t> lastNumbers_;
  std::optional<std::uint32_t> chipsBeforeDraw_;
};

}  // namespace rtype::client
