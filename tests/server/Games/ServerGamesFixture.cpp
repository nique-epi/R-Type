#include "ServerGamesFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConfigurationStep.hpp"
#include "ConnectedPlayer.hpp"
#include "GamePlayers.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "LoopStages.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "ReceivedDatagrams.hpp"
#include "RecordingNetworkModule.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerGamesModule.hpp"
#include "ServerLoggerModule.hpp"
#include "ServerModuleTestConstants.hpp"
#include "ServerStorageModule.hpp"
#include "ServiceProbeModule.hpp"
#include "ShippedEntitySheets.hpp"
#include "TimeConstants.hpp"

using rtype::network::IncomingDatagram;
using rtype::server::ConnectedPlayer;
using rtype::server::testing::playerEndpoint;

void ServerGamesFixture::startWith(
    rtype::server::GamesSettings settings,
    rtype::engine::ConfigurationStep configuration) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerLoggerModule>(journalOptions()));
  auto network = std::make_unique<RecordingNetworkModule>();
  network_ = network.get();
  modules.push_back(std::move(network));
  modules.push_back(std::make_unique<rtype::server::ServerStorageModule>(
      databaseFolder_.path()));
  modules.push_back(
      std::make_unique<rtype::server::ServerGamesModule>(std::move(settings)));
  for (auto& moduleAfterTheGames : modulesAfterTheGames()) {
    modules.push_back(std::move(moduleAfterTheGames));
  }
  auto probe = std::make_unique<ServiceProbeModule>(
      PROBE_MODULE,
      std::vector<std::string>{std::string{rtype::server::GAMES_MODULE_NAME}},
      [](rtype::engine::Duration) {});
  probe_ = probe.get();
  modules.push_back(std::move(probe));
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules), std::move(configuration));
  application().start();
}

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerGamesFixture::modulesAfterTheGames() const {
  return {};
}

rtype::server::GamesSettings ServerGamesFixture::shippedWaves() {
  return rtype::server::GamesSettings{
      .seed = SOME_SEED,
      .wavesFile = std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} / "waves" /
                   "waves.toml",
      .entitySheetsFolder = rtype::game::testing::shippedSheetFolder()};
}

ConnectedPlayer& ServerGamesFixture::join(std::size_t index) {
  return seatPlayerIn(probe().runningGame()->game(), index);
}

void ServerGamesFixture::runTick(
    const std::vector<IncomingDatagram>& players,
    const std::vector<IncomingDatagram>& requests,
    const std::vector<IncomingDatagram>& informationRequests) {
  network().tickDatagrams().begin(rtype::server::ReceivedDatagrams{
      .players = players,
      .connectionRequests = requests,
      .informationRequests = informationRequests,
      .probes = {}});
  application().runStages(rtype::engine::SIMULATION_TICK_DURATION);
  network().tickDatagrams().end();
}

std::size_t ServerGamesFixture::datagramsTo(std::size_t index,
                                            rtype::network::MessageType type) {
  std::size_t count = 0;
  for (const auto& datagram : rtype::server::testing::sentTo(
           network().sender().sent(), playerEndpoint(index))) {
    for (const auto& carried :
         rtype::server::testing::carriedMessagesOf(datagram)) {
      if (carried.type == static_cast<std::uint8_t>(type)) {
        ++count;
      }
    }
  }
  return count;
}

std::uint64_t ServerGamesFixture::tokenOf(std::size_t index) {
  return PLAYER_TOKEN_BASE + index;
}

rtype::engine::Application& ServerGamesFixture::application() {
  return *application_;
}

RecordingNetworkModule& ServerGamesFixture::network() { return *network_; }

ServiceProbeModule& ServerGamesFixture::probe() { return *probe_; }
