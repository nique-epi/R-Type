#pragma once

#include <functional>
#include "IScreenNavigator.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief What HEADPHONES RECOMMENDED is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HeadphonesScreenParts {
  /** @brief Screens it lives among, which must outlive it: the next screen
   * replaces it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief The screen that replaces it, screenAfterTheHeadphones(). */
  ScreenId next;
};

}  // namespace rtype::client
