#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "ClothPlace.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "RouletteBets.hpp"
#include "RouletteRound.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief What the roulette table read in a frame, of the table, of the
 * player and of its own controls: everything it shows is computed from it
 * (rouletteViewOf()).
 */
struct RouletteReading {
  /** @brief The newest state of the table; none without one. */
  std::optional<network::RouletteRound> round = std::nullopt;
  /** @brief What is left of the phase of the round, counted down since the
   * state arrived. */
  engine::Duration timeLeft{};
  std::uint8_t ownPlayerNumber{};
  /** @brief The pilots of the waiting room, for their names; valid until the
   * next update of the session. */
  std::span<const network::LobbyPlayer> players;
  /** @brief The chips of the settings, and those the player held when the
   * bets closed, which the settings no longer show once the round is
   * settled. */
  std::uint32_t chipsHeld{};
  std::optional<std::uint32_t> chipsBeforeDraw = std::nullopt;
  /** @brief The bets of the player for the round, and whether it asked to
   * spin. */
  std::vector<network::RouletteBet> bets;
  bool spinAsked{};
  /** @brief The bets of the round before, which REBET lays again. */
  std::vector<network::RouletteBet> previousBets;
  std::vector<std::uint8_t> lastNumbers;
  ClothPlace cursor;
  /** @brief The place of the chip chosen in CHIP_VALUES. */
  std::size_t chosenChip{};
  PlayerPalette palette{};
  MotionPreference motion{};
};

}  // namespace rtype::client
