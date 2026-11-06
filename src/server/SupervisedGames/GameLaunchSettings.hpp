#pragma once

#include <filesystem>

namespace rtype::server {

/** @brief What every game process the supervisor starts has in common. */
struct GameLaunchSettings {
  /** @brief The program that runs a game: the executable of the supervisor
   * itself, which a game process runs as a server. */
  std::filesystem::path program;
  /** @brief The folder the journal of each game process goes to, and the
   * leaderboard they all write to: the file a server on the port of the
   * supervisor would use (DATABASE_FILE_PREFIX, the port, then
   * DATABASE_FILE_SUFFIX). */
  std::filesystem::path folder;
};

}  // namespace rtype::server
