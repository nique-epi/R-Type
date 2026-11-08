#pragma once

#include <functional>
#include "IAudioService.hpp"
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"
#include "MotionPreference.hpp"
#include "PlayerPalette.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief What the hangar is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HangarScreenParts {
  /** @brief Screens the hangar lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the locker, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Plays the garage ambience under the theme while the hangar
   * lives; it must outlive the screen. */
  std::reference_wrapper<IAudioService> audio;
  /** @brief Holds and saves what the ship wears; it must outlive the screen.
   */
  std::reference_wrapper<SettingsService> settings;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
  /** @brief The palette the ship is painted in: a constant. */
  std::reference_wrapper<const PlayerPalette> palette;
  /** @brief The pilot color of the ship: the player's once a server accepted
   * them, the first one before. */
  int shipColor{};
  /** @brief Whether the arms, the ship and the beacons may move. */
  MotionPreference motion{};
};

}  // namespace rtype::client
