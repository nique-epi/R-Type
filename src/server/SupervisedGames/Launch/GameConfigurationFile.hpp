#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

namespace rtype::server {

/** @returns The file of commands the supervisor on @p supervisorPort writes
 * for its games, in @p folder: r-type_games_<port>.cfg. */
[[nodiscard]] std::filesystem::path gameConfigurationFileOf(
    const std::filesystem::path& folder, std::uint16_t supervisorPort);

/**
 * @brief Writes @p lines to @p file, one per line, in their order, replacing
 * what the file held.
 * @throws GameConfigurationWriteException when the file cannot be written.
 */
void writeGameConfiguration(const std::filesystem::path& file,
                            std::span<const std::string> lines);

}  // namespace rtype::server
