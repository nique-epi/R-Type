#include "ServerSupervisedGamesModule.hpp"
#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ChildrenStopper.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
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
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PathText.hpp"
#include "ReceivedStatus.hpp"
#include "ServerConstants.hpp"
#include "StopPause.hpp"
#include "SupervisedGame.hpp"
#include "SupervisedGamesConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @returns Where the game on @p port listens, on the loopback. */
network::Endpoint loopbackEndpoint(std::uint16_t port) {
  return network::Endpoint{
      .address = std::string{network::LOOPBACK_IPV4_ADDRESS}, .port = port};
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
  sender_ = &context.require<IDatagramSender>();
  admission_ = &context.require<IPlayerAdmission>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  listeningPort_ = &context.require<IListeningPort>();
  spawner_.emplace(GameSpawnerParts{.launcher = *launcher_,
                                    .registry = registry_,
                                    .admission = *admission_,
                                    .supervisorPort = *listeningPort_,
                                    .settings = settings_,
                                    .freePort = freeUdpPort});
  context.provide(registry_);
  context.provide(*spawner_);
  context.addWork(SIMULATION_STAGE, [this](engine::Duration elapsed) {
    superviseGames(elapsed);
  });
}

void ServerSupervisedGamesModule::start() {
  const std::filesystem::path file = gameConfigurationFileOf(
      settings_.folder, listeningPort_->listeningPort());
  const std::span<const std::string> lines =
      settings_.gameLines == nullptr ? std::span<const std::string>{}
                                     : std::span{*settings_.gameLines};
  writeGameConfiguration(file, lines);
  logger_.info("configuration of the games written to ",
               logging::genericText(file), ": ", lines.size(), " lines");
}

void ServerSupervisedGamesModule::stop() {
  ChildrenStopper stopper{*sender_, pause_};
  reportExits(stopper.stopAll(registry_));
}

void ServerSupervisedGamesModule::superviseGames(engine::Duration elapsed) {
  reportExits(registry_.collectExits());
  readStatuses();
  registry_.terminateStale(elapsed);
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
