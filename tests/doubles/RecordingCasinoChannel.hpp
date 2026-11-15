#pragma once

#include <cstddef>
#include <optional>
#include <vector>
#include "CasinoPlace.hpp"
#include "ChipMoves.hpp"
#include "ICasinoChannel.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"

/**
 * @brief Casino channel double that writes down every place chosen and every
 * bet laid, in order, and whose round and chip moves the test sets: a real
 * channel changes them on its own, at any time between two updates.
 */
class RecordingCasinoChannel : public rtype::client::ICasinoChannel {
 public:
  /** @brief Writes the place down. */
  void chooseTable(rtype::network::TablePlace place) override;
  /** @brief Writes the bets down; the vote of the spin is ignored. */
  void placeBets(std::vector<rtype::network::RouletteBet> bets,
                 bool spinVote) override;
  [[nodiscard]] std::optional<rtype::network::RouletteRound> round()
      const override;
  /** @brief Hands over the moves set with setChipMoves(), once. */
  [[nodiscard]] rtype::client::ChipMoves takeChipMoves() override;

  void setRound(std::optional<rtype::network::RouletteRound> round);
  void setChipMoves(rtype::client::ChipMoves moves);

  /** @returns The places chooseTable() was called with, in order. */
  [[nodiscard]] const std::vector<rtype::network::TablePlace>& chosenPlaces()
      const;
  /** @returns How many times placeBets() was called. */
  [[nodiscard]] std::size_t betsLaid() const;

 private:
  std::optional<rtype::network::RouletteRound> round_;
  rtype::client::ChipMoves chipMoves_{};
  std::vector<rtype::network::TablePlace> chosenPlaces_;
  std::size_t betsLaid_{0};
};
