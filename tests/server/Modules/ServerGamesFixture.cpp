#include "ServerGamesFixture.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "GamesSettings.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "LoopStages.hpp"
#include "MessageType.hpp"
#include "ModuleConstants.hpp"
#include "PacketHeader.hpp"
#include "SentDatagrams.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerGamesModule.hpp"
#include "ServerJournalModule.hpp"
#include "ServerModuleTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::network::IncomingDatagram;
using rtype::server::ConnectedPlayer;
using rtype::server::testing::playerEndpoint;

void ServerGamesFixture::startWith(rtype::server::GamesSettings settings) {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules;
  modules.push_back(
      std::make_unique<rtype::server::ServerJournalModule>(journalOptions()));
  auto network = std::make_unique<RecordingNetworkModule>();
  network_ = network.get();
  modules.push_back(std::move(network));
  modules.push_back(
      std::make_unique<rtype::server::ServerGamesModule>(std::move(settings)));
  auto probe = std::make_unique<ServiceProbeModule>(
      PROBE_MODULE,
      std::vector<std::string>{std::string{rtype::server::GAMES_MODULE_NAME}},
      [](rtype::engine::Duration) {});
  probe_ = probe.get();
  modules.push_back(std::move(probe));
  application_ = std::make_unique<rtype::engine::Application>(
      rtype::engine::LoopStages(rtype::server::SERVER_LOOP_STAGES),
      std::move(modules));
  application_->start();
}

rtype::server::GamesSettings ServerGamesFixture::shippedWaves() {
  return rtype::server::GamesSettings{
      .seed = SOME_SEED,
      .wavesFile = std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} / "waves" /
                   "waves.toml"};
}

ConnectedPlayer& ServerGamesFixture::join(std::size_t index) {
  rtype::server::GameInstance& game = probe_->games()->joinable();
  ConnectedPlayer* const player = game.roster().add(
      playerEndpoint(index), std::string{PLAYER_NICKNAME}, tokenOf(index));
  game.onPlayerJoined(*player);
  return *player;
}

void ServerGamesFixture::runTick(const std::vector<IncomingDatagram>& players) {
  network_->tickDatagrams().begin(
      rtype::server::ServerApplication::ReceivedDatagrams{
          .players = players, .connectionRequests = {}});
  application_->runStages(rtype::engine::SIMULATION_TICK_DURATION);
  network_->tickDatagrams().end();
}

IncomingDatagram ServerGamesFixture::disconnectionFrom(
    std::size_t index) const {
  rtype::network::PacketHeader header;
  header.sessionToken = tokenOf(index);
  return IncomingDatagram{.sender = playerEndpoint(index),
                          .payload = disconnectionDatagram(header)};
}

std::size_t ServerGamesFixture::datagramsTo(std::size_t index,
                                            rtype::network::MessageType type) {
  std::size_t count = 0;
  for (const auto& datagram : rtype::server::testing::sentTo(
           network_->sender().sent(), playerEndpoint(index))) {
    for (const auto& frame :
         rtype::network::DatagramCodec::splitMessages(datagram.payload)) {
      if (frame.type == static_cast<std::uint8_t>(type)) {
        ++count;
      }
    }
  }
  return count;
}

std::uint64_t ServerGamesFixture::tokenOf(std::size_t index) {
  return PLAYER_TOKEN_BASE + index;
}
