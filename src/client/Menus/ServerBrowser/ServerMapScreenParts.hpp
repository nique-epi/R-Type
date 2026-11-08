#pragma once

#include <functional>
#include "IGameCatalog.hpp"
#include "IMasterAddress.hpp"
#include "IScreenNavigator.hpp"
#include "IServerCatalog.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"
#include "ServerChoice.hpp"

namespace rtype::client {

/**
 * @brief What a server map is built from, but the canvas it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member without a
 * default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct ServerMapScreenParts {
  /** @brief Screens it lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the chips, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The servers to show, and the games of the chosen one; they must
   * outlive the screen. */
  std::reference_wrapper<IServerCatalog> servers;
  std::reference_wrapper<IGameCatalog> games;
  /** @brief Where the server a player joins goes, with the nickname the
   * connection screen joins with; it must outlive the screen. */
  std::reference_wrapper<ServerChoice> choice;
  /** @brief The master the servers are read from, written under the chip
   * MASTER; it must outlive the screen. */
  std::reference_wrapper<const IMasterAddress> master;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Whether the planet turns. */
  MotionPreference motion{};
};

}  // namespace rtype::client
