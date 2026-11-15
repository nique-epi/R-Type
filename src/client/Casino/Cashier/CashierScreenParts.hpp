#pragma once

#include <functional>
#include "CashierWay.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief What the cashier is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct CashierScreenParts {
  /** @brief Screens the cashier lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the clicks, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Holds and saves the kopecks and the chips of the player; it must
   * outlive the screen. */
  std::reference_wrapper<SettingsService> settings;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief Whether it changes kopecks into chips, or chips back. */
  CashierWay way{};
};

}  // namespace rtype::client
