#include "GameAssetPathsFixtures.hpp"
#include <array>
#include <filesystem>
#include <span>
#include <string>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScripts.hpp"
#include "ContentFolders.hpp"
#include "EntitySheets.hpp"
#include "GameAssetPaths.hpp"
#include "GameAssetPathsTestConstants.hpp"
#include "ResourcePlaces.hpp"
#include "SheetTextReading.hpp"
#include "ShippedEntitySheets.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The waves of @p folders, read with their entity sheets and the
 * behaviours their scripts register, as the games module reads them. */
WaveCatalog wavesOf(std::span<const std::filesystem::path> folders) {
  game::BehaviourScripts scripts{[](const std::string&) {}};
  scripts.run(game::scriptSourcesIn(folders));
  const game::EntitySheets types = game::EntitySheets::fromTexts(
      game::sheetTextsInFolders(folders), scripts.behaviours());
  return WaveCatalog::fromFiles(wavesFilesOf(folders), types);
}

}  // namespace

WaveCatalog wavesWithTheExampleResource() {
  const std::filesystem::path assets = game::testing::shippedAssetFolder();
  const engine::ResourcePlaces places{
      .assetFolder = assets,
      .resourceFolder = assets / RESOURCE_FOLDER_FROM_THE_ASSETS};
  const std::array names{std::string{EXAMPLE_RESOURCE_NAME}};
  return wavesOf(engine::contentFoldersOf(places, names));
}

WaveCatalog wavesOfTheAssets() {
  const std::array folders{game::testing::shippedAssetFolder()};
  return wavesOf(folders);
}

}  // namespace rtype::server::testing
