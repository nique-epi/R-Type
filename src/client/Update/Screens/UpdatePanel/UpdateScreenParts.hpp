#pragma once

#include <functional>
#include <string>
#include "IScreenNavigator.hpp"
#include "IUpdateRun.hpp"

namespace rtype::client {

/**
 * @brief What the update screen is built from, but the picture it owns.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct UpdateScreenParts {
  /** @brief Screens the panel lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief The update it shows, which must outlive it. */
  std::reference_wrapper<IUpdateRun> run;
  /** @brief The line under the heading: VER 1.0.0 > VER 1.1.0. */
  std::string versionLine;
};

}  // namespace rtype::client
