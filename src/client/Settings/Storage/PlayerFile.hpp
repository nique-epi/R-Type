#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include "Logger.hpp"
#include "PlayerFileWrite.hpp"

namespace rtype::client {

/**
 * @brief Reads a file the client keeps for the player (the settings, the
 * session), whole.
 * @returns Its text; nothing when there is no file, when it is larger than
 * @p maximumSize or cannot be read, each but the first logged on @p logger as
 * a warning.
 */
[[nodiscard]] std::optional<std::string> readPlayerFile(
    const std::filesystem::path& file, std::uintmax_t maximumSize,
    const logging::Logger& logger);

/**
 * @brief Replaces a file the client keeps for the player: writes the text
 * next to the file, with its permissions set before the text is, then
 * renames it over the file, so the file is either the previous one or the
 * new one. The folder is created first when missing, open to the player only.
 * @returns Whether the file now holds the text; a failure is logged on
 * @p logger as a warning and leaves the previous file, if any.
 */
[[nodiscard]] bool replacePlayerFile(const PlayerFileWrite& write,
                                     const logging::Logger& logger);

}  // namespace rtype::client
