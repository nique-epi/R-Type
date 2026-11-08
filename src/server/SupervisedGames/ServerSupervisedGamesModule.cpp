#include "ServerSupervisedGamesModule.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ChildrenStopper.hpp"
#include "ConfigurationRegistry.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntitySheets.hpp"
#include "ErrorText.hpp"
#include "ExitedGame.hpp"
#include "FreeUdpPort.hpp"
#include "GameConfigurationFile.hpp"
#include "GameLaunchSettings.hpp"
#include "GameLifecycle.hpp"
#include "GameSpawnerParts.hpp"
#include "GameStatusAcknowledged.hpp"
#include "GameStatusReading.hpp"
#include "IDatagramSender.hpp"
#include "IListeningPort.hpp"
#include "IPlayerAdmission.hpp"
#include "IProcessLauncher.hpp"
#include "IResultsFile.hpp"
#include "IServerOccupancy.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PathText.hpp"
#include "ReceivedStatus.hpp"
#include "ServerConstants.hpp"
#include "ServerDatabaseFile.hpp"
#include "SettingValue.hpp"
#include "StopPause.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"
#include "WaveCatalog.hpp"

namespace rtype::server {

namespace {

/** @returns Where the game on @p port listens, on the loopback. */
network::Endpoint loopbackEndpoint(std::uint16_t port) {
  return network::Endpoint{
      .address = std::string{network::LOOPBACK_IPV4_ADDRESS}, .port = port};
}

/** @returns The names of the difficulties of the waves file of @p settings,
 * read with the entity sheets it names its enemies among.
 * @throws what EntitySheets::fromFolder() and WaveCatalog::fromFile() throw. */
std::vector<std::string> difficultyNamesOf(const GameLaunchSettings& settings) {
  const game::EntitySheets types =
      game::EntitySheets::fromFolder(settings.entitySheetsFolder);
  return WaveCatalog::fromFile(settings.wavesFile, types).difficulties();
}

/** @returns The database setting, declared in @p configuration. */
const engine::SettingValue& declareDatabase(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{DATABASE_SETTING_NAME},
      .defaultValue = {},
      .help = "SQLite file the leaderboard of the games of this server is "
              "kept in; empty for the file of its port next to the executable",
      .validate = {}});
}

}  // namespace

ServerSupervisedGamesModule::ServerSupervisedGamesModule(
    GameLaunchSettings settings, std::unique_ptr<IProcessLauncher> launcher,
    StopPause pause)
    : settings_(std::move(settings)),
      launcher_(std::move(launcher)),
      pause_(std::move(pause)),
      logger_(std::string{SUPERVISOR_LOGGER_NAME}) {}

std::string ServerSupervisedGamesModule::name() const {
  return std::string{SUPERVISED_GAMES_MODULE_NAME};
}

std::vector<std::string> ServerSupervisedGamesModule::dependencies() const {
  return {std::string{NETWORK_MODULE_NAME}};
}

void ServerSupervisedGamesModule::install(engine::InstallContext& context) {
  database_ =
      &declareDatabase(context.require<engine::ConfigurationRegistry>());
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  listeningPort_ = &context.require<IListeningPort>();
  spawner_.emplace(GameSpawnerParts{.launcher = *launcher_,
                                    .registry = registry_,
                                    .admission = *admission_,
                                    .supervisorPort = *listeningPort_,
                                    .settings = settings_,
                                    .database = *database_,
                                    .freePort = freeUdpPort});
  context.provide(registry_);
  context.provide(difficulties_);
  context.provide(*spawner_);
  context.provide<IServerOccupancy>(watch_);
  context.provide<IResultsFile>(*this);
  context.provide(watch_.hostedGames());
  context.provide(watch_.measures());
  context.addWork(SIMULATION_STAGE, [this](engine::Duration elapsed) {
    superviseGames(elapsed);
  });
}

void ServerSupervisedGamesModule::start() {
  difficulties_.keep(difficultyNamesOf(settings_));
  logger_.info("difficulties of the games: ",
               engine::listText(difficulties_.names()));
  const std::filesystem::path file = gameConfigurationFileOf(
      settings_.folder, listeningPort_->listeningPort());
  writeGameConfiguration(file, settings_.gameLines);
  logger_.info("configuration of the games written to ",
               logging::genericText(file), ": ", settings_.gameLines.size(),
               " lines");
}

void ServerSupervisedGamesModule::stop() {
  ChildrenStopper stopper{*sender_, pause_};
  reportExits(stopper.stopAll(registry_));
}

std::filesystem::path ServerSupervisedGamesModule::resultsFile() const {
  return databaseFileOf(database_->text(), settings_.folder,
                        listeningPort_->listeningPort());
}

void ServerSupervisedGamesModule::superviseGames(engine::Duration elapsed) {
  reportExits(registry_.collectExits());
  readStatuses();
  registry_.terminateStale(elapsed);
  watch_.publishIfChanged();
}

void ServerSupervisedGamesModule::readStatuses() {
  for (const network::IncomingDatagram& datagram : tickDatagrams_->players()) {
    const std::optional<ReceivedStatus> received = readGameStatus(datagram);
    if (!received.has_value()) {
      continue;
    }
    SupervisedGame* const game = registry_.findByToken(received->token);
    if (game == nullptr) {
      continue;
    }
    const bool wasStarting = game->lifecycle == GameLifecycle::Starting;
    if (registry_.record(*game, received->sequence, received->status)) {
      if (wasStarting) {
        logger_.info("game ", game->number, " is ready");
      }
      answerStatus(*game);
    }
  }
}

void ServerSupervisedGamesModule::answerStatus(const SupervisedGame& game) {
  sender_->queueDatagram(network::OutgoingDatagram{
      .destination = loopbackEndpoint(game.port),
      .payload = network::DatagramCodec::encode(
          network::PacketHeader{.sessionToken = game.linkToken},
          network::GameStatusAcknowledged{})});
}

void ServerSupervisedGamesModule::reportExits(
    const std::vector<ExitedGame>& exits) {
  for (const ExitedGame& exited : exits) {
    admission_->forget(loopbackEndpoint(exited.port), exited.linkToken);
    if (exited.crashed) {
      logger_.error("game ", exited.number, " crashed while ",
                    lifecycleText(exited.lifecycle), ", exit code ",
                    exited.exitCode);
    } else {
      logger_.info("game ", exited.number, " ended");
    }
  }
}

}  // namespace rtype::server
