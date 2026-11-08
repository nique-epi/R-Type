#include "ServerSupervisedGamesFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConnectionChallenge.hpp"
#include "ConnectionRequest.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "GameLaunchSettings.hpp"
#include "GameRules.hpp"
#include "GameStatus.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "LoopStages.hpp"
#include "Network/ReceivedDatagrams.hpp"
#include "PacketHeader.hpp"
#include "RecordingNetworkModule.hpp"
#include "RecordingProcessLauncher.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerSupervisedGamesModule.hpp"
#include "ServerSupervisorLobbyModule.hpp"
#include "ShippedEntitySheets.hpp"
#include "SupervisedGamesFixtures.hpp"
#include "SupervisedGamesTestConstants.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

using rtype::network::IncomingDatagram;
using rtype::server::testing::onlyMessageIn;
using rtype::server::testing::sentTo;

void ServerSupervisedGamesFixture::startSupervisor(
    std::vector<std::string> gameLines) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerLoggerModule>(journalOptions()));
  auto network = std::make_unique<RecordingNetworkModule>();
  network_ = network.get();
  modules.push_back(std::move(network));
  auto launcher = std::make_unique<RecordingProcessLauncher>();
  launcher_ = launcher.get();
  modules.push_back(
      std::make_unique<rtype::server::ServerSupervisedGamesModule>(
          rtype::server::GameLaunchSettings{
              .program = PROGRAM_FOR_TESTS,
              .folder = folder_.path(),
              .gameLines = std::move(gameLines),
              .forwardedArguments = {},
              .wavesFile = std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
                           rtype::server::WAVE_FILE_IN_ASSETS,
              .contentFolders = {rtype::game::testing::shippedAssetFolder()}},
          std::move(launcher), [](rtype::engine::Duration) {}));
  modules.push_back(
      std::make_unique<rtype::server::ServerSupervisorLobbyModule>());
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules));
  application_->start();
}

void ServerSupervisedGamesFixture::stopSupervisor() { application_->stop(); }

void ServerSupervisedGamesFixture::runTick(
    const std::vector<IncomingDatagram>& players,
    const std::vector<IncomingDatagram>& requests) {
  network().tickDatagrams().begin(
      rtype::server::ReceivedDatagrams{.players = players,
                                       .connectionRequests = requests,
                                       .informationRequests = {},
                                       .probes = {}});
  application_->runStages(rtype::engine::SIMULATION_TICK_DURATION);
  network().tickDatagrams().end();
}

void ServerSupervisedGamesFixture::runFor(rtype::engine::Duration duration) {
  for (rtype::engine::Duration done = rtype::engine::Duration::zero();
       done < duration; done += rtype::engine::SIMULATION_TICK_DURATION) {
    runTick({});
  }
}

void ServerSupervisedGamesFixture::askForAPlaceAs(
    std::size_t index, const rtype::network::GameChoice& game,
    const std::optional<rtype::network::GameRules>& rules) {
  const rtype::network::Endpoint player = supervisedPlayer(index);
  runTick({},
          {IncomingDatagram{.sender = player,
                            .payload = requestDatagramFor(game, 0, rules)}});
  const auto answers = sentTo(network().sender().sent(), player);
  const auto cookie =
      onlyMessageIn<rtype::network::ConnectionChallenge>(answers.back()).cookie;
  runTick({}, {IncomingDatagram{
                  .sender = player,
                  .payload = requestDatagramFor(game, cookie, rules)}});
}

void ServerSupervisedGamesFixture::gameReports(
    std::size_t game, const rtype::network::GameStatus& status,
    std::uint16_t sequence) {
  const auto& command = launcher().commands().at(game);
  const auto port =
      static_cast<std::uint16_t>(std::stoi(argumentAfter(command, "-p")));
  const std::uint64_t token =
      std::stoull(argumentAfter(command, "--link-token"));
  runTick(
      {IncomingDatagram{.sender = loopbackOn(port),
                        .payload = rtype::network::DatagramCodec::encode(
                            rtype::network::PacketHeader{.sessionToken = token,
                                                         .sequence = sequence},
                            status)}});
}

std::filesystem::path ServerSupervisedGamesFixture::gameConfigurationFile()
    const {
  return folder_.path() / GAMES_FILE_OF_PORT_4242;
}

std::string ServerSupervisedGamesFixture::gameConfigurationText() const {
  std::ifstream stream{gameConfigurationFile()};
  return std::string{std::istreambuf_iterator<char>{stream},
                     std::istreambuf_iterator<char>{}};
}

RecordingProcessLauncher& ServerSupervisedGamesFixture::launcher() {
  return *launcher_;
}

RecordingNetworkModule& ServerSupervisedGamesFixture::network() {
  return *network_;
}
