#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "ClothPlace.hpp"
#include "OtherPlayerBets.hpp"
#include "RouletteBand.hpp"
#include "RouletteBet.hpp"
#include "SpinCountdown.hpp"
#include "TableRosterRow.hpp"
#include "WheelPicture.hpp"

namespace rtype::client {

/** @brief What the roulette table paints but its bars: the croupier, the
 * wheel, the chips of the player on the cloth, the cursor while the bets are
 * open, the number drawn at the result, the players seated under the name of
 * the room, the numbers drawn before, and the band. */
struct RouletteView {
  /** @brief The frame of the croupier on the sheet of the butler, cut at
   * CROUPIER_HEIGHT. */
  sf::IntRect croupier;
  /** @brief The plaque under the croupier, while the bets are open. */
  std::optional<SpinCountdown> countdown;
  /** @brief The bets of the other players seated in the room. */
  std::vector<OtherPlayerBets> others;
  WheelPicture wheel;
  std::vector<network::RouletteBet> bets;
  std::optional<ClothPlace> cursor = std::nullopt;
  std::optional<std::uint8_t> drawn = std::nullopt;
  /** @brief The name of the room, written as the tag of the panel of the
   * players; empty without a room. */
  std::string roomName;
  std::vector<TableRosterRow> roster;
  std::vector<std::uint8_t> lastNumbers;
  RouletteBand band;
};

}  // namespace rtype::client
