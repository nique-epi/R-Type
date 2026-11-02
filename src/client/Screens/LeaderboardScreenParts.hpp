#pragma once

#include <functional>
#include <vector>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "KeyNames.hpp"
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardTab.hpp"
#include "MotionPreference.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

/**
 * @brief What a leaderboard screen is built from, but the canvas it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is only
 * ever built with designated initializers naming every member without a
 * default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LeaderboardScreenParts {
  /** @brief Screens it lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief The tabs, left to right, at least one; the first opens. */
  std::vector<LeaderboardTab> tabs;
  /** @brief The filter it opens on. */
  LeaderboardFilterChoice choice;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Gives the name a keycap shows for a key. */
  KeyNaming keyName;
  /** @brief Whether the banners and the guards may move. */
  MotionPreference motion{};
};

}  // namespace rtype::client
