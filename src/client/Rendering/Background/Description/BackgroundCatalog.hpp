#pragma once

#include <filesystem>
#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <vector>
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
   * @brief Reads the backgrounds of the last of @p contentFolders: the
   * assets when they are alone, or what is laid over the folders before it,
   * as the pack of a server over the assets. Each `.toml` file of its
   * BACKGROUND_FOLDER_IN_ASSETS folder, when it has one, is a background
   * named after the file, as readBackground() reads it. The backgrounds of
   * the folders before it are not read.
   *
   * Each file a background names is looked for from the last folder to the
   * first. Found in the first folder, the assets, it keeps its asset id;
   * found in another, it is named by its whole path, which the asset library
   * loads as it is.
   *
   * @param contentFolders The assets folder, then what is laid over it;
   * never empty.
   * @throws engine::UnreadableDataFileException when the folder of
   * backgrounds cannot be listed or a file cannot be read; what
   * readBackground() throws, naming the file by its absolute path; and
   * MissingBackgroundFileException when a background names a file no folder
   * holds.
   */
  [[nodiscard]] static BackgroundCatalog ofLastFolder(
      std::span<const std::filesystem::path> contentFolders);

  /** @returns The background named @p name, or nullptr when no folder holds
   * one. */
  [[nodiscard]] const BackgroundDescription* named(std::string_view name) const;

  /** @returns The name of every background, in alphabetical order. */
  [[nodiscard]] std::vector<std::string> names() const;

  /** @returns Every image a layer of a background names, as the catalog
   * names it, each once, in alphabetical order: the images of the layers,
   * the tilesets, the sprites of the particles. The client loads them before
   * the sky draws them. */
  [[nodiscard]] const std::vector<std::string>& images() const;

 private:
  explicit BackgroundCatalog(
      std::map<std::string, BackgroundDescription, std::less<>> backgrounds);

  std::map<std::string, BackgroundDescription, std::less<>> backgrounds_;
  std::vector<std::string> images_;
};

}  // namespace rtype::client
