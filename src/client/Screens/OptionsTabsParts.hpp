#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include "ISizableWindow.hpp"
#include "ISoundPlayer.hpp"
#include "KeyNames.hpp"
#include "OptionsPlace.hpp"
#include "SettingsService.hpp"

namespace rtype::client {

/**
 * @brief What the tabs of the options are built from: the services their rows
 * read and change, and what the screen does when a tab is asked for, BACK is
 * pressed or a reset must be confirmed. The service, the sounds and the
 * window must outlive the tabs.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct OptionsTabsParts {
  std::reference_wrapper<SettingsService> service;
  std::reference_wrapper<ISizableWindow> window;
  /** @brief Plays the click of the menus. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief What the options are opened over. */
  OptionsPlace place{};
  /** @brief Gives the name drawn on the keycap of a key. */
  KeyNaming keyName;
  /** @brief Called with the tab a click or a key asks for. */
  std::function<void(std::size_t)> openTab;
  /** @brief Called when BACK is pressed. */
  std::function<void()> close;
  /** @brief Called with a question and the reset to run if it is answered YES. */
  std::function<void(const std::string&, const std::function<void()>&)>
      askToReset;
};

}  // namespace rtype::client
