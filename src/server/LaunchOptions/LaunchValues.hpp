#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace rtype::server {

/**
 * @brief What the launch options of the supervisor set while the arguments are
 * read: each option writes its own part.
 */
struct LaunchValues {
  /** @brief The lines of the command `set` the options force, in order. */
  std::vector<std::string> forcedLines;
  /** @brief The file the traffic of the program goes to, given after
   * `--traffic-csv`; nothing when no file was asked for. */
  std::optional<std::filesystem::path> trafficCsvFile;
  /** @brief Whether `--help` was given. */
  bool helpRequested{false};
};

}  // namespace rtype::server
