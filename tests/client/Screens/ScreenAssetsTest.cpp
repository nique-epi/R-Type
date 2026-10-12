#include <gtest/gtest.h>
#include <filesystem>
#include "AssetLibrary.hpp"
#include "AssetLoadingStep.hpp"
#include "ScreenAssets.hpp"
#include "ScreenConstants.hpp"

using rtype::client::AssetLibrary;
using rtype::client::AssetLoadingStep;
using rtype::client::gameAssets;
using rtype::client::INTERFACE_FONT_ID;
using rtype::client::loadInterfaceAssets;

namespace {

std::filesystem::path repositoryAssets() { return RTYPE_ASSETS_FOLDER; }

}  // namespace

/**
 * Given the assets folder of the repository
 * When the interface assets are loaded at launch
 * Then the font of the screens can be looked up
 */
TEST(ScreenAssets, LaunchLoadsTheInterfaceFont) {
  AssetLibrary library{repositoryAssets()};

  loadInterfaceAssets(library);

  EXPECT_NO_THROW(static_cast<void>(library.font(INTERFACE_FONT_ID)));
}

/**
 * Given the interface assets loaded at launch
 * When the loading step of a game starts
 * Then the font of the screens is still loaded
 */
TEST(ScreenAssets, EnteringAGameKeepsTheInterfaceFont) {
  AssetLibrary library{repositoryAssets()};
  loadInterfaceAssets(library);

  const AssetLoadingStep gameLoading(library, gameAssets());

  EXPECT_NO_THROW(static_cast<void>(library.font(INTERFACE_FONT_ID)));
}
