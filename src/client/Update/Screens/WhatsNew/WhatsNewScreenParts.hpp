#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ReleaseNotes.hpp"

namespace rtype::client {

/**
 * @brief What WHAT'S NEW is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct WhatsNewScreenParts {
  /** @brief Screens it lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief The notes shown. */
  ReleaseNotes notes;
  /** @brief Run once the player closed it: the notes are then read. */
  std::function<void()> onClose;
};

}  // namespace rtype::client
