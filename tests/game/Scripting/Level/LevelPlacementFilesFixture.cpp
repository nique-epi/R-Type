#include "LevelPlacementFilesFixture.hpp"
#include <filesystem>
#include <span>
#include <string_view>
#include "BehaviourScriptFiles.hpp"
#include "BehaviourScriptsFixtures.hpp"
#include "LdtkPlacementFixtures.hpp"
#include "LevelPlacementFiles.hpp"
#include "LevelPlacementFilesTestConstants.hpp"
#include "LuauLevels.hpp"
#include "ShippedEntitySheets.hpp"

LevelPlacementFilesFixture::LevelPlacementFilesFixture()
    : scripts_(reportInto(log_)) {}

void LevelPlacementFilesFixture::writeContent(std::string_view content,
                                              std::string_view entities) {
  root_.write(std::filesystem::path{content} / PLACED_LEVEL_SCRIPT,
              REGISTERS_THE_PLACED_LEVEL);
  root_.write(std::filesystem::path{content} / PLACED_LEVEL_LDTK,
              oneLevelProject(entities));
}

std::filesystem::path LevelPlacementFilesFixture::contentFolder(
    std::string_view content) const {
  return root_.path() / content;
}

void LevelPlacementFilesFixture::readPlacements(
    std::span<const std::filesystem::path> folders) {
  scripts_.run(rtype::game::scriptSourcesIn(folders));
  rtype::game::readLevelPlacements(scripts_.levels(), folders,
                                   rtype::game::testing::shippedEntitySheets());
}

rtype::game::LuauLevels& LevelPlacementFilesFixture::levels() {
  return scripts_.levels();
}
