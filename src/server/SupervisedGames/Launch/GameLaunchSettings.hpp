#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

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
   * its games, read before its modules are built; none gives the games an
   * empty file. */
  std::vector<std::string> gameLines;
  /** @brief The options of the launch of the supervisor every game is given
   * as they are (see LaunchOptions::gameArguments()): the logging options it
   * shares and the network simulated. */
  std::vector<std::string> forwardedArguments;
  /** @brief The waves file the games read, which the supervisor reads too
   * when it starts, for the names of the difficulties its games may be played
   * at. */
  std::filesystem::path wavesFile;
  /** @brief The content folders the games are made from, the assets first
   * then each resource their `ensure` lines start (see
   * engine::contentFoldersOf()): the waves file names its enemies among their
   * entity sheets. */
  std::vector<std::filesystem::path> contentFolders;
  /** @brief The file given to the supervisor after `--traffic-csv`, which
   * names the traffic file of each game; nothing: the games write none. */
  std::optional<std::filesystem::path> trafficCsvFile = std::nullopt;
};

}  // namespace rtype::server
