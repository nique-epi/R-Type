#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include "CasinoRefusal.hpp"
#include "CasinoRoomState.hpp"
#include "ClothPlace.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "RouletteBet.hpp"
#include "RoundBets.hpp"

namespace rtype::client {

/**
 * @brief What the roulette table read in a frame, of the room, of the player
 * and of its own controls: everything it shows is computed from it
 * (rouletteViewOf()).
 */
struct RouletteReading {
  /** @brief The newest state of the room, timed on the clock of the master
   * (ICasinoChannel::room()); none before the first. */
  std::optional<network::CasinoRoomState> room = std::nullopt;
  /** @brief The nickname of the player, as the room lists it. */
  std::string ownNickname;
  /** @brief The chips of the wallet, and those the player may lay on the
   * round in progress: the chips left and those the master holds on the
   * cloth. */
  std::uint32_t walletChips{};
  std::uint32_t chipsAtHand{};
  /** @brief The bets of the player for the round, and the chips of those the
   * master took. */
  std::vector<network::RouletteBet> bets;
  std::uint32_t acceptedStake{};
  /** @brief The bets the player held on the round, and what they paid, once
   * the master answered. */
  std::optional<network::RoundBets> ownBets = std::nullopt;
  /** @brief Why the master refused the newest bets of the round, or the
   * seat. */
  std::optional<CasinoRefusal> refusal = std::nullopt;
  /** @brief The bets of the round before, which REBET lays again. */
  std::vector<network::RouletteBet> previousBets;
  ClothPlace cursor;
  /** @brief The place of the chip chosen in CHIP_VALUES. */
  std::size_t chosenChip{};
  PlayerPalette palette{};
  MotionPreference motion{};
};

}  // namespace rtype::client
