#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISizableWindow.hpp"
#include "ISoundPlayer.hpp"
#include "KeyNames.hpp"
#include "OptionsPlace.hpp"
#include "PlayfieldMapping.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief What the options are built from, but the picture they own. The
 * navigator, the settings service and the window must outlive the screen.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct OptionsScreenParts {
  /** @brief Screens the options live among. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the menu, which must outlive the screen. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief What the options are opened over. */
  OptionsPlace place{};
  /** @brief What the rows read, change through and save with. */
  std::reference_wrapper<SettingsService> settings;
  /** @brief Window whose display mode and size DISPLAY picks. */
  std::reference_wrapper<ISizableWindow> window;
  /** @brief Gives the name drawn on the keycap of a key. */
  KeyNaming keyName;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
};

}  // namespace rtype::client
