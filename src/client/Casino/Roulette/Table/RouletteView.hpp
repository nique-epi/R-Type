#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "ClothPlace.hpp"
#include "RouletteBand.hpp"
#include "RouletteBets.hpp"
#include "TableRosterRow.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

/** @brief What the roulette table paints but its bars: the croupier, the
 * wheel, the chips of the player on the cloth, the cursor while the bets are
 * open, the number drawn at the result, the pilots seated, the numbers drawn
 * before, and the band. */
struct RouletteView {
  /** @brief Whether the croupier invites the bets, or waits. */
  bool croupierInvites{};
  WheelPicture wheel;
  std::vector<network::RouletteBet> bets;
  std::optional<ClothPlace> cursor = std::nullopt;
  std::optional<std::uint8_t> drawn = std::nullopt;
  std::vector<TableRosterRow> roster;
  std::vector<std::uint8_t> lastNumbers;
  RouletteBand band;
};

}  // namespace rtype::client
