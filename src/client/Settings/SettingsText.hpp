#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "PlayerSettings.hpp"

namespace rtype::client {

/** @brief What reading a settings text gave: the settings, and each line
 * left out, as "line 3: unknown setting". */
struct SettingsReading {
  PlayerSettings settings;
  std::vector<std::string> ignoredLines;
};

/**
 * @returns The text of the settings file: SETTINGS_FILE_COMMENT, the version,
 * then one NAME=VALUE line per setting; no window size line while
 * windowSize is empty.
 */
[[nodiscard]] std::string settingsText(const PlayerSettings& settings);

/**
 * @brief Reads a settings text, written by settingsText() or by hand.
 *
 * @param defaults Settings a missing, unknown or invalid line leaves as they
 * are; each such line is listed in the reading.
 */
[[nodiscard]] SettingsReading readSettingsText(std::string_view text,
                                               const PlayerSettings& defaults);

}  // namespace rtype::client
