#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ISoundPlayer.hpp"

namespace rtype::client {

/**
 * @brief What the shortcuts of the waiting room are built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member without
 * a default: the member-init checks, which report its implicit default
 * constructor, see a constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct RoomShortcutsParts {
  /** @brief Screens the room lives among, which must outlive the
   * shortcuts. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Plays the click of the chips, which must outlive them. */
  std::reference_wrapper<ISoundPlayer> sounds;
  /** @brief Run before a shortcut pushes its screen: the room drops what it
   * held, since the events stop reaching it. */
  std::function<void()> beforeOpening;
  /** @brief The column the row of the shortcuts ends at, on the right. */
  int right{};
};

}  // namespace rtype::client
