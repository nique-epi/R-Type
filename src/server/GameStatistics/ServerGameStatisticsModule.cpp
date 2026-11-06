#include "ServerGameStatisticsModule.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "CsvFile.hpp"
#include "EventBus.hpp"
#include "ExecutablePath.hpp"
#include "GameStatisticsConstants.hpp"
#include "GameTally.hpp"
#include "IGameBuses.hpp"
#include "IListeningPort.hpp"
#include "ILogJournal.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "PathText.hpp"
#include "ServerConstants.hpp"
#include "TextSettingDefinition.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

std::string ServerGameStatisticsModule::name() const {
  return std::string{GAME_STATISTICS_MODULE_NAME};
}

std::vector<std::string> ServerGameStatisticsModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{GAMES_MODULE_NAME}};
}

void ServerGameStatisticsModule::install(engine::InstallContext& context) {
  logger_.emplace(std::string{GAME_STATISTICS_LOGGER_NAME},
                  context.require<logging::ILogJournal>());
  fileSetting_ = &context.require<engine::ConfigurationRegistry>().declareText(
      engine::TextSettingDefinition{
          .name = std::string{GAME_STATISTICS_FILE_SETTING_NAME},
          .defaultValue = {},
          .help = "CSV file the statistics of the games are named after: "
                  "each game writes <name>_<its port><extension>; empty for "
                  "r-type_statistics_<its port>.csv next to the executable",
          .validate = {}});
  port_ = &context.require<IListeningPort>();
  buses_ = &context.require<IGameBuses>();
  context.addWork(OUTPUT_STAGE,
                  [this](engine::Duration elapsed) { onTick(elapsed); });
}

void ServerGameStatisticsModule::start() {
  const std::filesystem::path file = statisticsFile();
  file_.emplace(file, GAME_STATISTICS_CSV_HEADER);
  sinceWrite_ = engine::Duration::zero();
  followEveryGame();
  if (logger_.has_value()) {
    logger_->info("game statistics written to ", logging::genericText(file));
  }
}

void ServerGameStatisticsModule::stop() {
  tallies_.clear();
  writeRows();
  file_.reset();
}

std::filesystem::path ServerGameStatisticsModule::statisticsFile() const {
  const std::string port = std::to_string(port_->listeningPort());
  if (fileSetting_->text().empty()) {
    return engine::executableFolder() /
           (std::string{GAME_STATISTICS_FILE_PREFIX} + port +
            std::string{GAME_STATISTICS_FILE_SUFFIX});
  }
  const std::filesystem::path configured{fileSetting_->text()};
  std::filesystem::path name = configured.stem();
  name += GAME_STATISTICS_PORT_SEPARATOR;
  name += port;
  name += configured.extension();
  return configured.parent_path() / name;
}

void ServerGameStatisticsModule::followEveryGame() {
  for (const std::unique_ptr<engine::EventBus>& events : buses_->buses()) {
    tallies_.push_back(std::make_unique<GameTally>(*events, sheet_));
  }
}

void ServerGameStatisticsModule::onTick(engine::Duration elapsed) {
  sinceWrite_ += elapsed;
  if (sinceWrite_ < GAME_STATISTICS_WRITE_PERIOD) {
    return;
  }
  sinceWrite_ = engine::Duration::zero();
  writeRows();
}

void ServerGameStatisticsModule::writeRows() {
  const std::string rows = sheet_.takeRows();
  if (rows.empty() || !file_.has_value() || file_->append(rows)) {
    return;
  }
  file_.reset();
  if (logger_.has_value()) {
    logger_->warn(
        "the game statistics file cannot be written: no more rows will be");
  }
}

}  // namespace rtype::server
