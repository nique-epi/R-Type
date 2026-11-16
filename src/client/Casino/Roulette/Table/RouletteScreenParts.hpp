#pragma once

#include <functional>
#include "ICasinoChannel.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "IWaitingRoom.hpp"
#include "IWallet.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "RouletteMemory.hpp"

namespace rtype::client {

/**
 * @brief What the roulette table is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct RouletteScreenParts {
  /** @brief Screens the table lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The table the bets go to; it must outlive the screen. */
  std::reference_wrapper<ICasinoChannel> casino;
  /** @brief The bets of the round and the numbers drawn, which outlive the
   * screen, so the player finds them when it comes back. */
  std::reference_wrapper<RouletteMemory> memory;
  /** @brief The wallet of the account signed in, read at every update for
   * the chips; it must outlive the screen. */
  std::reference_wrapper<const IWallet> wallet;
  /** @brief Tells the number of the player; it must outlive the screen. */
  std::reference_wrapper<const IServerConnection> connection;
  /** @brief Tells the names of the pilots; it must outlive the screen. */
  std::reference_wrapper<const IWaitingRoom> waitingRoom;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief The colors of the pilots, as the player chose them. */
  PlayerPalette palette{};
  /** @brief Whether the wheel turns. */
  MotionPreference motion{};
};

}  // namespace rtype::client
