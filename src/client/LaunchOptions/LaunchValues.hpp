#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "AudioMode.hpp"
#include "Endpoint.hpp"

namespace rtype::client {

/**
 * @brief What the launch options of the client set while the arguments are
 * read: each option writes its own part.
 */
struct LaunchValues {
  /** @brief The server to join. */
  network::Endpoint serverEndpoint;
  /** @brief The nickname of the player, UTF-8. */
  std::string nickname;
  /** @brief Whether the client makes sound. */
  AudioMode audioMode{AudioMode::Enabled};
  /** @brief The file the traffic of the program goes to, given after
   * `--traffic-csv`; nothing when no file was asked for. */
  std::optional<std::filesystem::path> trafficCsvFile;
  /** @brief The lines of the command `set` the options force, in order. */
  std::vector<std::string> forcedLines;
  /** @brief Whether `--help` was given. */
  bool helpRequested{false};
};

}  // namespace rtype::client
