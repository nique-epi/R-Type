#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "CsvFile.hpp"
#include "GameTally.hpp"
#include "IGameEvents.hpp"
#include "IListeningPort.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "Logger.hpp"
#include "SettingValue.hpp"
#include "StatisticsSheet.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The module of the game statistics plugin: it writes to a CSV file a
 * row for each Bydo a player kills, naming the game, the player and the
 * points, then the total of each game that ends, so a host balances the waves
 * from real games.
 *
 * It is built as a shared library of its own and loaded by
 * `plugin game-statistics`: no list of the server names it, and the server
 * behaves the same without it. It follows the game through IGameEvents, so its
 * library holds none of its code. install() declares the
 * `game-statistics-file` setting and adds its work to the outputs stage, after
 * the work of the games module, which delivers the events of the tick there.
 * start() opens the file, emptied, then subscribes to the bus of the game. The
 * rows are kept in memory and written once GAME_STATISTICS_WRITE_PERIOD of game
 * time passed since the last write; stop() writes the rows left and closes the
 * file, so the file is complete once the server stopped, after Ctrl-C too. A
 * file that stops accepting rows is told once in the journal and left, so a
 * full disk never stops a game.
 */
class ServerGameStatisticsModule final : public engine::IModule {
 public:
  /** @returns GAME_STATISTICS_MODULE_NAME. */
  [[nodiscard]] std::string name() const override;

  /** @returns The logger, the network and the games: the journal, the port
   * that names the file by default, and the bus of the game. */
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @brief Declares the `game-statistics-file` setting, requires the journal,
   * the port and the bus, and adds its work to the outputs stage.
   * @throws engine::MissingServiceException when one of them is missing.
   */
  void install(engine::InstallContext& context) override;

  /**
   * @brief Opens the file, emptied, then follows the bus of the game.
   * @throws network::CsvFileOpenException when the file cannot be opened;
   * nothing is followed then.
   */
  void start() override;

  /** @brief Stops following the game, writes the rows kept and closes the
   * file. */
  void stop() override;

 private:
  /** @returns The file the setting names with the port of the game before
   * its extension (stats.csv gives stats_<port>.csv), or
   * r-type_statistics_<port>.csv next to the executable while it is empty: a
   * supervisor runs several games, which listen on several ports, so two
   * games never write the same file. */
  [[nodiscard]] std::filesystem::path statisticsFile() const;

  /** @brief Adds @p elapsed, and writes the rows kept once
   * GAME_STATISTICS_WRITE_PERIOD passed, then starts the count over. */
  void onTick(engine::Duration elapsed);

  /** @brief Writes the rows kept, while the file accepts them. */
  void writeRows();

  std::optional<logging::Logger> logger_;
  const engine::SettingValue* fileSetting_{nullptr};
  const IListeningPort* port_{nullptr};
  IGameEvents* game_{nullptr};
  StatisticsSheet sheet_;
  /** @brief Follows the bus of the game from start() until stop() destroys
   * it. */
  std::optional<GameTally> tally_;
  std::optional<network::CsvFile> file_;
  engine::Duration sinceWrite_{};
};

}  // namespace rtype::server
