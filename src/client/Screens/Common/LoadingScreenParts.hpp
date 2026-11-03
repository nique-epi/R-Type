#pragma once

#include <functional>
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "IScreenNavigator.hpp"
#include "ScreenId.hpp"

namespace rtype::client {

/**
 * @brief What a loading screen is built from.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LoadingScreenParts {
  /** @brief Screens the loading screen lives among, which must outlive it. */
  std::reference_wrapper<IScreenNavigator> navigator;
  /** @brief Library the assets are loaded into, which must outlive the
   * screen. */
  std::reference_wrapper<AssetLibrary> library;
  /** @brief Assets the next screen needs; read by the constructor only. */
  std::reference_wrapper<const AssetList> assets;
  /** @brief Screen shown once every asset is loaded. */
  ScreenId nextScreen;
};

}  // namespace rtype::client
