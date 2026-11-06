#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include "SettingValue.hpp"

namespace rtype::server {

/** @brief What every game process the supervisor starts has in common. */
struct GameLaunchSettings {
  /** @brief The program that runs a game: the executable of the supervisor
   * itself, which a game process runs as a server. */
  std::filesystem::path program;
  /** @brief The folder the journal of each game process goes to, and the
   * file of commands of the games. The leaderboard they all write to is
   * there too while the database setting is empty: DATABASE_FILE_PREFIX, the
   * port of the supervisor, then DATABASE_FILE_SUFFIX. */
  std::filesystem::path folder;
  /** @brief The lines of the configuration of the supervisor that belong to
   * its games, which its configuration step fills before the modules start;
   * they must outlive the module. Nothing: the games get an empty file. */
  const std::vector<std::string>* gameLines{nullptr};
  /** @brief The database setting of the supervisor, which its module declares:
   * the leaderboard every game writes to when it is not empty. Nothing, or an
   * empty text: the file of the port in folder. It must outlive the module. */
  const engine::SettingValue* database{nullptr};
};

}  // namespace rtype::server
