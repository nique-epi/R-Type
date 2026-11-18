#include "BackgroundCatalog.hpp"
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>
#include "BackgroundDescription.hpp"
#include "BackgroundFileConstants.hpp"
#include "BackgroundReading.hpp"
#include "BackgroundText.hpp"
#include "ClientException.hpp"
#include "DataFileReading.hpp"
#include "ImageContent.hpp"
#include "LayerDescription.hpp"
#include "MapContent.hpp"
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "PathText.hpp"

namespace rtype::client {

namespace {

using NamedFiles = std::vector<std::reference_wrapper<std::string>>;

/** @returns The files @p image names: its image. */
NamedFiles namedFilesOf(ImageContent& image) { return {image.file}; }

/** @returns The files @p maps names: its project and its tileset. */
NamedFiles namedFilesOf(MapContent& maps) {
  return {maps.project, maps.tileset};
}

/** @returns The files @p particles names: the image of each sprite. */
NamedFiles namedFilesOf(ParticleContent& particles) {
  NamedFiles files;
  for (ParticleSprite& sprite : particles.sprites) {
    files.emplace_back(sprite.file);
  }
  return files;
}

/** @brief Adds to @p images the image @p image names. */
void addImagesOf(const ImageContent& image, std::set<std::string>& images) {
  images.insert(image.file);
}

/** @brief Adds to @p images the tileset @p maps names. */
void addImagesOf(const MapContent& maps, std::set<std::string>& images) {
  images.insert(maps.tileset);
}

/** @brief Adds to @p images the image of each sprite of @p particles. */
void addImagesOf(const ParticleContent& particles,
                 std::set<std::string>& images) {
  for (const ParticleSprite& sprite : particles.sprites) {
    images.insert(sprite.file);
  }
}

/** @returns How the client names the file @p file once found in one of
 * @p contentFolders, the last first: its asset id in the first folder, its
 * whole path in another; nothing when no folder holds it. */
std::optional<std::string> locatedIn(
    std::span<const std::filesystem::path> contentFolders,
    const std::string& file) {
  for (const std::filesystem::path& folder :
       contentFolders | std::views::reverse) {
    const std::filesystem::path candidate =
        folder / std::filesystem::path{file};
    std::error_code ignored;
    if (std::filesystem::is_regular_file(candidate, ignored)) {
      return &folder == &contentFolders.front() ? file
                                                : engine::shownPath(candidate);
    }
  }
  return std::nullopt;
}

/** @brief Names each file @p layer names as locatedIn() finds it in
 * @p contentFolders. @throws MissingBackgroundFileException, naming the
 * background file @p source, for a file no folder holds. */
void locateFilesOf(LayerDescription& layer, std::string_view source,
                   std::span<const std::filesystem::path> contentFolders) {
  const NamedFiles files = std::visit(
      [](auto& content) { return namedFilesOf(content); }, layer.content);
  for (std::string& file : files) {
    std::optional<std::string> located = locatedIn(contentFolders, file);
    if (!located.has_value()) {
      throw MissingBackgroundFileException(std::string{source}, file);
    }
    file = std::move(*located);
  }
}

/** @returns The text of every background file of @p contentFolder, in the
 * order of their names; none when it has no folder of backgrounds. */
std::vector<BackgroundText> backgroundTextsIn(
    const std::filesystem::path& contentFolder) {
  const std::filesystem::path folder =
      contentFolder / BACKGROUND_FOLDER_IN_ASSETS;
  std::vector<BackgroundText> texts;
  std::error_code ignored;
  if (!std::filesystem::is_directory(folder, ignored)) {
    return texts;
  }
  for (const std::filesystem::path& file : engine::dataFilesIn(folder)) {
    texts.push_back(BackgroundText{.name = logging::genericText(file.stem()),
                                   .source = engine::shownPath(file),
                                   .text = engine::dataFileContent(file)});
  }
  return texts;
}

/** @returns Every image a layer of @p backgrounds names, each once, in
 * alphabetical order. */
std::vector<std::string> imagesOf(
    const std::map<std::string, BackgroundDescription, std::less<>>&
        backgrounds) {
  std::set<std::string> distinct;
  for (const BackgroundDescription& background :
       std::views::values(backgrounds)) {
    for (const LayerDescription& layer : background.layers) {
      std::visit(
          [&distinct](const auto& content) { addImagesOf(content, distinct); },
          layer.content);
    }
  }
  return {distinct.begin(), distinct.end()};
}

}  // namespace

BackgroundCatalog BackgroundCatalog::ofLastFolder(
    std::span<const std::filesystem::path> contentFolders) {
  std::map<std::string, BackgroundDescription, std::less<>> backgrounds;
  for (const BackgroundText& text : backgroundTextsIn(contentFolders.back())) {
    BackgroundDescription background = readBackground(text);
    for (LayerDescription& layer : background.layers) {
      locateFilesOf(layer, text.source, contentFolders);
    }
    backgrounds.insert_or_assign(text.name, std::move(background));
  }
  return BackgroundCatalog{std::move(backgrounds)};
}

const BackgroundDescription* BackgroundCatalog::named(
    std::string_view name) const {
  const auto found = backgrounds_.find(name);
  return found == backgrounds_.end() ? nullptr : &found->second;
}

std::vector<std::string> BackgroundCatalog::names() const {
  std::vector<std::string> names;
  names.reserve(backgrounds_.size());
  for (const std::string& name : std::views::keys(backgrounds_)) {
    names.push_back(name);
  }
  return names;
}

const std::vector<std::string>& BackgroundCatalog::images() const {
  return images_;
}

BackgroundCatalog::BackgroundCatalog(
    std::map<std::string, BackgroundDescription, std::less<>> backgrounds)
    : backgrounds_(std::move(backgrounds)), images_(imagesOf(backgrounds_)) {}

}  // namespace rtype::client
