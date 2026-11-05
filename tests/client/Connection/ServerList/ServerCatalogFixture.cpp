#include "ServerCatalogFixture.hpp"
#include <chrono>
#include <string>
#include <string_view>
#include "Endpoint.hpp"
#include "ListedServer.hpp"
#include "PingAnswer.hpp"
#include "ProtocolConstants.hpp"
#include "RecordingMasterReads.hpp"
#include "RecordingServerPingProbe.hpp"
#include "ServerCatalog.hpp"
#include "ServerCatalogTestConstants.hpp"
#include "ServerInformation.hpp"
#include "ServerListing.hpp"
#include "ServerRegistration.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

network::ListedServer ServerCatalogFixture::listed(ListedServerSketch sketch) {
  return network::ListedServer{
      .id = sketch.id,
      .registration =
          network::ServerRegistration{
              .name = std::string{sketch.address},
              .address = std::string{sketch.address},
              .udpPort = SERVER_UDP_PORT,
              .gameVersion = std::string{},
              .protocolVersion = network::PROTOCOL_VERSION,
              .maximumGames = SERVER_MAXIMUM_GAMES,
              .maximumPlayersPerGame = SERVER_PLAYERS_PER_GAME},
      .status = sketch.status,
      .gameCount = 0,
      .playerCount = 0};
}

network::Endpoint ServerCatalogFixture::endpointAt(std::string_view address) {
  return network::Endpoint{.address = std::string{address},
                           .port = SERVER_UDP_PORT};
}

void ServerCatalogFixture::listAndUpdate(
    const network::ServerListing& listing) {
  catalog_.refresh();
  master_.answerServers(listing);
  catalog_.update(ONE_FRAME);
}

void ServerCatalogFixture::answerFrom(std::string_view address,
                                      std::chrono::microseconds ping) {
  pings_.deliver(PingAnswer{.server = endpointAt(address),
                            .roundTrip = ping,
                            .information = network::ServerInformation{}});
}

void ServerCatalogFixture::update(engine::Duration elapsed) {
  catalog_.update(elapsed);
}

ServerCatalog& ServerCatalogFixture::catalog() { return catalog_; }

RecordingMasterReads& ServerCatalogFixture::master() { return master_; }

const RecordingServerPingProbe& ServerCatalogFixture::pings() const {
  return pings_;
}

}  // namespace rtype::client::testing
