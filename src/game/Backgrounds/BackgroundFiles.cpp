#include "BackgroundFiles.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <toml++/toml.hpp>
#include <utility>
#include <vector>
#include "BackgroundFileKeyConstants.hpp"
#include "ContentFile.hpp"
#include "ContentFolders.hpp"
#include "DataFileConstants.hpp"
#include "DataFileReading.hpp"
#include "DataFileTable.hpp"
#include "EngineException.hpp"
#include "PathText.hpp"

namespace rtype::game {

namespace {

/** @brief The bytes of the files of a listing, by their path in a content
 * folder. */
using BytesByPath = std::map<std::string, std::string, std::less<>>;

/** @returns The background files of @p resourceFolders, each by its path in
 * a content folder: the one of the last resource that holds that name. */
std::map<std::string, std::filesystem::path, std::less<>> backgroundFilesOf(
    std::span<const std::filesystem::path> resourceFolders) {
  std::map<std::string, std::filesystem::path, std::less<>> files;
  for (const std::filesystem::path& resource : resourceFolders) {
    const std::filesystem::path folder = resource / BACKGROUND_FOLDER_IN_ASSETS;
    std::error_code ignored;
    if (!std::filesystem::is_directory(folder, ignored)) {
      continue;
    }
    for (const std::filesystem::path& file : engine::dataFilesIn(folder)) {
      files.insert_or_assign(
          logging::genericText(
              std::filesystem::path{BACKGROUND_FOLDER_IN_ASSETS} /
              file.filename()),
          file);
    }
  }
  return files;
}

/** @returns The text of each of @p keys in the table @p content of
 * @p layer; none when the layer has no such table. */
std::vector<std::string> textsOf(const engine::DataFileTable& layer,
                                 std::string_view content,
                                 std::span<const std::string_view> keys) {
  std::vector<std::string> texts;
  const std::optional<engine::DataFileTable> table =
      engine::optionalTableAt(layer, content);
  if (table.has_value()) {
    for (const std::string_view key : keys) {
      texts.push_back(engine::textAt(*table, key));
    }
  }
  return texts;
}

/** @returns The image of each sprite of the particles of @p layer; none for
 * a layer without particles, or with particles drawn without sprites. */
std::vector<std::string> spriteFilesOf(const engine::DataFileTable& layer) {
  std::vector<std::string> files;
  const std::optional<engine::DataFileTable> particles =
      engine::optionalTableAt(layer, BACKGROUND_PARTICLES_KEY);
  if (particles.has_value() &&
      particles->table.get().contains(BACKGROUND_SPRITES_KEY)) {
    for (const engine::DataFileTable& sprite :
         engine::tablesAt(*particles, BACKGROUND_SPRITES_KEY)) {
      files.push_back(engine::textAt(sprite, BACKGROUND_FILE_KEY));
    }
  }
  return files;
}

/** @brief Adds @p added at the end of @p names. */
void append(std::vector<std::string>& names, std::vector<std::string> added) {
  std::ranges::move(added, std::back_inserter(names));
}

/** @returns Every file the background whose text is @p text names, in the
 * order of its layers: the image of a layer, the project and the tileset of
 * tiles or props, the image of each sprite of particles. The errors call the
 * background @p source. */
std::vector<std::string> namedFilesOf(const std::string& text,
                                      const std::string& source) {
  const toml::table root = engine::parseDataFile(text, source);
  std::vector<std::string> names;
  for (const engine::DataFileTable& layer : engine::tablesAt(
           engine::rootOf(root, source), BACKGROUND_LAYER_TABLES_KEY)) {
    append(names,
           textsOf(layer, BACKGROUND_IMAGE_KEY, BACKGROUND_IMAGE_FILE_KEYS));
    append(names,
           textsOf(layer, BACKGROUND_TILES_KEY, BACKGROUND_MAP_FILE_KEYS));
    append(names,
           textsOf(layer, BACKGROUND_PROPS_KEY, BACKGROUND_MAP_FILE_KEYS));
    append(names, spriteFilesOf(layer));
  }
  return names;
}

/**
 * @brief Adds to @p files the file @p name a background names, read from the
 * last resource of @p contentFolders that holds it; nothing when only the
 * assets hold it, or when it is listed already.
 * @throws engine::UnreadableDataFileException, naming the file in the
 * assets, when no folder holds it.
 */
void addNamedFile(BytesByPath& files,
                  std::span<const std::filesystem::path> contentFolders,
                  const std::string& name) {
  if (files.contains(name)) {
    return;
  }
  const std::filesystem::path inAssets =
      contentFolders.front() / std::filesystem::path{name};
  const std::filesystem::path lastHeld =
      engine::contentFilesOf(contentFolders, std::filesystem::path{name})
          .back();
  if (lastHeld != inAssets) {
    files.try_emplace(name, engine::dataFileContent(lastHeld));
    return;
  }
  std::error_code ignored;
  if (!std::filesystem::is_regular_file(inAssets, ignored)) {
    throw engine::UnreadableDataFileException(engine::shownPath(inAssets));
  }
}

}  // namespace

bool isBackgroundPath(const std::string& path) {
  const std::filesystem::path file{path};
  return file.parent_path() == BACKGROUND_FOLDER_IN_ASSETS &&
         file.extension() == engine::DATA_FILE_EXTENSION;
}

std::vector<identifiers::ContentFile> backgroundFilesIn(
    std::span<const std::filesystem::path> contentFolders) {
  BytesByPath bytes;
  for (const auto& [path, file] :
       backgroundFilesOf(contentFolders.subspan(1))) {
    const std::string text = engine::dataFileContent(file);
    bytes.insert_or_assign(path, text);
    for (const std::string& name :
         namedFilesOf(text, engine::shownPath(file))) {
      addNamedFile(bytes, contentFolders, name);
    }
  }
  std::vector<identifiers::ContentFile> files;
  files.reserve(bytes.size());
  for (auto& [path, content] : bytes) {
    files.push_back(
        identifiers::ContentFile{.path = path, .bytes = std::move(content)});
  }
  return files;
}

}  // namespace rtype::game
