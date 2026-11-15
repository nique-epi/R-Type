#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "PlayStart.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief What a destination screen is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct DestinationScreenParts {
  /** @brief Screens the destinations live among, which must outlive them. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Settings of the player, read at every repaint for the balance of
   * kopecks. They must outlive the screen. */
  std::reference_wrapper<const PlayerSettings> settings;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief PlayStart::GivenServer makes SERVER join the server given at
   * launch at once, instead of opening the server map. */
  PlayStart playStart{};
};

}  // namespace rtype::client
