#include "BackgroundCatalog.hpp"
#include <filesystem>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
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
#include "ParticleContent.hpp"
#include "ParticleSprite.hpp"
#include "PathText.hpp"
#include "PropContent.hpp"
#include "TileContent.hpp"

namespace rtype::client {

namespace {

using NamedFiles = std::vector<std::reference_wrapper<std::string>>;

/** @returns The files @p image names: its image. */
NamedFiles namedFilesOf(ImageContent& image) { return {image.file}; }

/** @returns The files @p tiles names: its project and its tileset. */
NamedFiles namedFilesOf(TileContent& tiles) {
  return {tiles.project, tiles.tileset};
}

/** @returns The files @p props names: its project and its tileset. */
NamedFiles namedFilesOf(PropContent& props) {
  return {props.project, props.tileset};
}

/** @returns The files @p particles names: the image of each sprite. */
NamedFiles namedFilesOf(ParticleContent& particles) {
  NamedFiles files;
  for (ParticleSprite& sprite : particles.sprites) {
    files.emplace_back(sprite.file);
  }
  return files;
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

}  // namespace

BackgroundCatalog BackgroundCatalog::fromFolders(
    std::span<const std::filesystem::path> contentFolders) {
  std::map<std::string, BackgroundDescription, std::less<>> backgrounds;
  for (const std::filesystem::path& folder : contentFolders) {
    for (const BackgroundText& text : backgroundTextsIn(folder)) {
      BackgroundDescription background = readBackground(text);
      for (LayerDescription& layer : background.layers) {
        locateFilesOf(layer, text.source, contentFolders);
      }
      backgrounds.insert_or_assign(text.name, std::move(background));
    }
  }
  return BackgroundCatalog{std::move(backgrounds)};
}

const BackgroundDescription* BackgroundCatalog::named(
    std::string_view name) const {
  const auto found = backgrounds_.find(name);
  return found == backgrounds_.end() ? nullptr : &found->second;
}

BackgroundCatalog::BackgroundCatalog(
    std::map<std::string, BackgroundDescription, std::less<>> backgrounds)
    : backgrounds_(std::move(backgrounds)) {}

}  // namespace rtype::client
