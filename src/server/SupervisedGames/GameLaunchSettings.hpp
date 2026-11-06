#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief What every game process the supervisor starts has in common. */
struct GameLaunchSettings {
  /** @brief The program that runs a game: the executable of the supervisor
   * itself, which a game process runs as a server. */
  std::filesystem::path program;
  /** @brief The folder the journal of each game process goes to, and the
   * leaderboard they all write to: the file a server on the port of the
   * supervisor would use (DATABASE_FILE_PREFIX, the port, then
   * DATABASE_FILE_SUFFIX). The file of commands of the games is written
   * there too. */
  std::filesystem::path folder;
  /** @brief The lines of the configuration of the supervisor that belong to
   * its games, which its configuration step fills before the modules start;
   * they must outlive the module. Nothing: the games get an empty file. */
  const std::vector<std::string>* gameLines{nullptr};
};

}  // namespace rtype::server
