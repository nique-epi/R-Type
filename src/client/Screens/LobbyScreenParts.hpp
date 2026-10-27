#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "LobbyPlace.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "ReadyKey.hpp"

namespace rtype::client {

/**
 * @brief What a waiting room is built from, but the canvas it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LobbyScreenParts {
  /** @brief Screens the waiting room lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu and of the ready key, which must
   * outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Session the waiting room reads and drives, which must outlive
   * it. */
  std::reference_wrapper<IServerConnection> connection;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  ReadyKey readyKey;
  /** @brief The palette the ships and their bands are painted in: a
   * constant, so it outlives the screen. */
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief Whether the ships and the beacons may move. */
  MotionPreference motion{};
  /** @brief Where BACK leads: the connection screen below, or the title. */
  LobbyPlace place{};
};

}  // namespace rtype::client
