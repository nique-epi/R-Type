#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISettingsStorage.hpp"
#include "ISizableWindow.hpp"
#include "KeyNames.hpp"
#include "OptionsPlace.hpp"
#include "PlayfieldMapping.hpp"

namespace rtype::client {

struct PlayerSettings;

/**
 * @brief What the options are built from, but the picture they own. The
 * navigator, the settings, the storage and the window must outlive the
 * screen.
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
  /** @brief What the options are opened over. */
  OptionsPlace place{};
  /** @brief What the rows read and change. */
  std::reference_wrapper<PlayerSettings> settings;
  /** @brief Where a change is saved. */
  std::reference_wrapper<ISettingsStorage> storage;
  /** @brief Window whose size DISPLAY picks. */
  std::reference_wrapper<ISizableWindow> window;
  /** @brief Gives the name drawn on the keycap of a key. */
  KeyNaming keyName;
  /** @brief Gives the point of the playfield under a pixel of the window. */
  PlayfieldMapping playfieldPointAt;
};

}  // namespace rtype::client
