#include "ResourceFixtures.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "ContentFoldersTestConstants.hpp"
#include "ResourcePlaces.hpp"
#include "TemporaryFolder.hpp"

std::vector<std::string> textsOf(std::span<const std::string_view> views) {
  return {views.begin(), views.end()};
}

rtype::engine::ResourcePlaces placesIn(const TemporaryFolder& root) {
  return rtype::engine::ResourcePlaces{
      .assetFolder = root.path() / TEST_ASSET_FOLDER,
      .resourceFolder = root.path() / TEST_RESOURCE_FOLDER};
}
