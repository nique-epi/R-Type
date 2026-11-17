#include "LdtkProjectFileFixtures.hpp"
#include <filesystem>
#include <string_view>
#include "LdtkProjectFileTestConstants.hpp"

std::filesystem::path shippedFile(std::string_view assetId) {
  return std::filesystem::path(RTYPE_SOURCE_ASSETS_FOLDER) /
         std::filesystem::path(assetId);
}

std::filesystem::path propProject() {
  return std::filesystem::path(RTYPE_LAYER_TEST_FOLDER) /
         std::filesystem::path(PROP_PROJECT_FILE);
}
