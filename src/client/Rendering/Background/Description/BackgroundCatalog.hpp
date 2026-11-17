#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include "BackgroundDescription.hpp"

namespace rtype::client {

/**
 * @brief The backgrounds the client can show, found by the name a level
 * gives them, each as its file describes it.
 *
 * The descriptions never move once read, so what draws a background may
 * refer to its description for as long as this object lives.
 */
class BackgroundCatalog {
 public:
  /**
   * @brief Reads the backgrounds of every content folder, the assets first,
   * then what is laid over them: each `.toml` file of the
   * BACKGROUND_FOLDER_IN_ASSETS folder of a content folder that has one,
   * named after its background, as readBackground() reads it. A later folder
   * replaces the background of the same name an earlier one holds, and adds
   * the others.
   *
   * Each file a background names is looked for from the last folder to the
   * first, so a later folder replaces an image too. Found in the first
   * folder, the assets, it keeps its asset id; found in another, it is named
   * by its whole path, which AssetLibrary::fileOf() keeps as it is.
   *
   * @throws engine::UnreadableDataFileException when a folder of backgrounds
   * cannot be listed or a file cannot be read; what readBackground() throws,
   * naming the file by its absolute path; and MissingBackgroundFileException
   * when a background names a file no folder holds.
   */
  [[nodiscard]] static BackgroundCatalog fromFolders(
      std::span<const std::filesystem::path> contentFolders);

  /** @returns The background named @p name, or nullptr when no folder holds
   * one. */
  [[nodiscard]] const BackgroundDescription* named(std::string_view name) const;

 private:
  explicit BackgroundCatalog(
      std::map<std::string, BackgroundDescription, std::less<>> backgrounds);

  std::map<std::string, BackgroundDescription, std::less<>> backgrounds_;
};

}  // namespace rtype::client
