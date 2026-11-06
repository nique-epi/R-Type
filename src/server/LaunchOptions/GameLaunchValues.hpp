#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace rtype::server {

/**
 * @brief What the launch options of a game process set while the arguments
 * are read: each option writes its own part.
 */
struct GameLaunchValues {
  /** @brief The lines of the command `set` the options force, in order. */
  std::vector<std::string> forcedLines;
  /** @brief The file the traffic of the game goes to, given after
   * `--traffic-csv`; nothing when no file was asked for. */
  std::optional<std::filesystem::path> trafficCsvFile;
  /** @brief The file of commands given after `--configuration`; nothing when
   * the game reads no file. */
  std::optional<std::filesystem::path> configurationFile;
  /** @brief The port given after `--supervisor`; nothing until it is read. */
  std::optional<std::uint16_t> supervisorPort;
  /** @brief The number given after `--link-token`; nothing until it is
   * read. */
  std::optional<std::uint64_t> linkToken;
  /** @brief Whether `--help` was given. */
  bool helpRequested{false};
};

}  // namespace rtype::server
