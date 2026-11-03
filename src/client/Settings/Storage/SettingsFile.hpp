#pragma once

#include <filesystem>
#include <optional>
#include "ISettingsStorage.hpp"
#include "Logger.hpp"
#include "PlayerSettings.hpp"

namespace rtype::client {

/**
 * @brief The settings kept in a text file, SettingsText's format.
 *
 * A save writes the whole text next to the file, then renames it over the
 * file, so the file is either the previous one or the new one. The folder is
 * created on the first save, readable by the player only. A file with no
 * path, as when the system tells no settings folder, never saves.
 */
class SettingsFile : public ISettingsStorage {
 public:
  /** @param path The file, or nothing to keep the settings for this launch
   * only. */
  explicit SettingsFile(std::optional<std::filesystem::path> path);

  /**
   * @returns The settings the file holds, over the defaults: the defaults
   * alone when there is no file, when it is larger than
   * MAXIMUM_SETTINGS_FILE_SIZE or cannot be read. Each line left out is
   * logged as a warning.
   */
  [[nodiscard]] PlayerSettings load(const PlayerSettings& defaults) const;

  bool save(const PlayerSettings& settings) override;

 private:
  /** @brief Writes the text to a file, replacing it. @returns Whether the
   * whole text was written and the file closed. */
  [[nodiscard]] static bool write(const std::filesystem::path& file,
                                  const PlayerSettings& settings);

  std::optional<std::filesystem::path> path_;
  logging::Logger logger_;
};

}  // namespace rtype::client
