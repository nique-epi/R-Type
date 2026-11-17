#include "BackgroundCatalogFixture.hpp"
#include <array>
#include <filesystem>
#include <string>
#include <string_view>
#include <variant>
#include "BackgroundCatalog.hpp"
#include "BackgroundCatalogTestConstants.hpp"
#include "BackgroundDescription.hpp"
#include "ImageContent.hpp"

using rtype::client::BackgroundCatalog;
using rtype::client::BackgroundDescription;
using rtype::client::ImageContent;

void BackgroundCatalogFixture::write(const std::filesystem::path& file,
                                     std::string_view content) const {
  folder_.write(file, content);
}

BackgroundCatalog BackgroundCatalogFixture::catalogOfTheAssets() const {
  const std::array folders{folder_.path() / ASSET_FOLDER};
  return BackgroundCatalog::fromFolders(folders);
}

BackgroundCatalog BackgroundCatalogFixture::catalogOfBothFolders() const {
  const std::array folders{folder_.path() / ASSET_FOLDER,
                           folder_.path() / RESOURCE_FOLDER};
  return BackgroundCatalog::fromFolders(folders);
}

std::string BackgroundCatalogFixture::wholePathOf(std::string_view file) const {
  return std::filesystem::absolute(folder_.path() / file).generic_string();
}

std::string firstLayerNameOf(const BackgroundDescription* background) {
  if (background == nullptr || background->layers.empty()) {
    return {};
  }
  return background->layers.front().name;
}

std::string firstImageFileOf(const BackgroundDescription* background) {
  if (background == nullptr || background->layers.empty()) {
    return {};
  }
  const auto* const image =
      std::get_if<ImageContent>(&background->layers.front().content);
  return image == nullptr ? std::string{} : image->file;
}

BackgroundCatalog shippedBackgrounds() {
  const std::array folders{std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER}};
  return BackgroundCatalog::fromFolders(folders);
}
