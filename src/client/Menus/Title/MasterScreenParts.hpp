#pragma once

#include <functional>
#include "IMasterAddress.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief What a master screen is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct MasterScreenParts {
  /** @brief Screens the form lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Where SAVE keeps the master, which must outlive the screen. */
  std::reference_wrapper<SettingsService> settings;
  /** @brief The master read when the form opens, which its fields start
   * with; it must outlive the screen. */
  std::reference_wrapper<const IMasterAddress> master;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
};

}  // namespace rtype::client
