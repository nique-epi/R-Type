#include "ServerModulesFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <utility>
#include <vector>
#include "Application.hpp"
#include "ConnectionChallenge.hpp"
#include "Endpoint.hpp"
#include "GameInstanceTestConstants.hpp"
#include "GameListSettings.hpp"
#include "GamesModule.hpp"
#include "IModule.hpp"
#include "IncomingDatagram.hpp"
#include "LobbyModule.hpp"
#include "LoopStages.hpp"
#include "MessageType.hpp"
#include "OutgoingDatagram.hpp"
#include "RecordingServerLink.hpp"
#include "SentDatagrams.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "ServerDatagrams.hpp"
#include "ServerTrafficModule.hpp"
#include "TimeConstants.hpp"
#include "WaveCatalogTestConstants.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The waves file shipped in the assets of the repository. */
std::filesystem::path shippedWaveFile() {
  return std::filesystem::path{RTYPE_SOURCE_ASSETS_FOLDER} /
         SHIPPED_FILE_IN_ASSETS;
}

/**
 * @returns A ServerTrafficModule providing @p link and @p received, then the
 * games playing SOME_SEED from the shipped waves file, then the lobby.
 */
std::vector<std::unique_ptr<engine::IModule>> trafficGamesThenLobby(
    RecordingServerLink& link, ServerApplication::ReceivedDatagrams& received) {
  std::vector<std::unique_ptr<engine::IModule>> modules;
  modules.push_back(std::make_unique<ServerTrafficModule>(link, received));
  modules.push_back(std::make_unique<GamesModule>(
      GameListSettings{.seed = SOME_SEED, .waveFile = shippedWaveFile()}));
  modules.push_back(std::make_unique<LobbyModule>());
  return modules;
}

/** @returns The type of each datagram of @p datagrams that goes to
 * @p destination, oldest first. */
std::vector<network::MessageType> messageTypesTo(
    std::span<const network::OutgoingDatagram> datagrams,
    const network::Endpoint& destination) {
  std::vector<network::MessageType> types;
  for (const network::OutgoingDatagram& datagram :
       sentTo(datagrams, destination)) {
    types.push_back(onlyMessageTypeOf(datagram));
  }
  return types;
}

}  // namespace

ServerModulesFixture::ServerModulesFixture()
    : application_(engine::LoopStages(SERVER_LOOP_STAGES),
                   trafficGamesThenLobby(link_, received_)) {
  application_.start();
}

void ServerModulesFixture::tick(
    std::vector<network::IncomingDatagram> players,
    std::vector<network::IncomingDatagram> requests) {
  players_ = std::move(players);
  requests_ = std::move(requests);
  received_ = ServerApplication::ReceivedDatagrams{
      .players = players_, .connectionRequests = requests_};
  application_.runStages(engine::SIMULATION_TICK_DURATION);
  received_ = ServerApplication::ReceivedDatagrams{};
  link_.sendAtEndOfTick();
}

void ServerModulesFixture::seatPlayer(std::size_t index) {
  tick({}, {network::IncomingDatagram{.sender = playerEndpoint(index),
                                      .payload = requestDatagram()}});
  const std::vector<network::OutgoingDatagram> answers =
      sentTo(link_.queued(), playerEndpoint(index));
  ASSERT_FALSE(answers.empty());
  const std::uint64_t cookie =
      onlyMessageIn<network::ConnectionChallenge>(answers.back()).cookie;
  tick({}, {network::IncomingDatagram{.sender = playerEndpoint(index),
                                      .payload = requestDatagram(cookie)}});
}

RecordingServerLink& ServerModulesFixture::link() { return link_; }

engine::Application& ServerModulesFixture::application() {
  return application_;
}

std::vector<network::MessageType> ServerModulesFixture::messageTypesQueuedFor(
    const network::Endpoint& destination) const {
  return messageTypesTo(link_.queued(), destination);
}

std::vector<network::MessageType>
ServerModulesFixture::messageTypesSentAfterRunTo(
    const network::Endpoint& destination) const {
  return messageTypesTo(link_.sentAfterRun(), destination);
}

std::vector<network::Endpoint> ServerModulesFixture::admittedEndpoints() const {
  std::vector<network::Endpoint> endpoints;
  for (const RecordedSession& session : link_.admitted()) {
    endpoints.push_back(session.endpoint);
  }
  return endpoints;
}

}  // namespace rtype::server::testing
