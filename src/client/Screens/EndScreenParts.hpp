#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include "FlashPreference.hpp"
#include "GameResult.hpp"
#include "IScreenNavigator.hpp"
#include "IServerConnection.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What an end screen is built from, but the canvas it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EndScreenParts {
  /** @brief Screens the end screen lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the sounds of the book and the click of a choice, which
   * must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The session, left when the player quits; it must outlive the
   * screen. */
  std::reference_wrapper<IServerConnection> connection;
  /** @brief How the game ended, read when the screen is built. */
  std::reference_wrapper<const GameResult> result;
  /** @brief The index in TAUNTS of the taunt of the previous end screen, if
   * any: the screen never picks it, and writes its own pick there. */
  std::reference_wrapper<std::optional<std::size_t>> previousTaunt;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Seed the taunt is drawn from. */
  std::uint32_t seed{};
  MotionPreference motion{};
  FlashPreference flashes{};
};

}  // namespace rtype::client
