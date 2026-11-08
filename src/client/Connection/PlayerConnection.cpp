#include "PlayerConnection.hpp"
#include <string>
#include <thread>
#include "ConnectionConstants.hpp"
#include "ContentPackCache.hpp"
#include "ContentPlaces.hpp"
#include "Endpoint.hpp"
#include "EntityAppearances.hpp"
#include "IEntityTypeTable.hpp"
#include "IPlayerRoundTrips.hpp"
#include "IServerPingProbe.hpp"
#include "IServerTicks.hpp"
#include "ISessionArrivals.hpp"
#include "InstallContext.hpp"
#include "LocalContentReading.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ServerSessionParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Every IPv4 interface, on the port the system picks. */
network::Endpoint anyInterfaceAnyPort() {
  return network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                           .port = network::ANY_PORT};
}

}  // namespace

PlayerConnection::PlayerConnection(const network::NetworkConditions& conditions,
                                   const ContentPlaces& places)
    : network_(anyInterfaceAnyPort(), conditions),
      router_(network_),
      orderedInbox_(dispatcher_),
      waitingRoom_(dispatcher_, gameLoaded_),
      shipLooks_(dispatcher_, waitingRoom_),
      entityMessages_(dispatcher_, waitingRoom_, gameLoaded_),
      roundTrips_(dispatcher_, clock_),
      leaderboard_(dispatcher_),
      chat_(dispatcher_, waitingRoom_),
      entityTypes_(localContentOf(places.assetFolder)),
      content_(ContentPackCache{places.cacheFolder}),
      session_(ServerSessionParts{
          .link = router_.addRoute(),
          .dispatcher = dispatcher_,
          .gameLoaded = gameLoaded_,
          .entityTypes = entityTypes_,
          .content = content_,
          .arrivals = arrivals_,
          .observers = {waitingRoom_, shipLooks_, entityMessages_, gameLoaded_,
                        roundTrips_, leaderboard_, chat_, orderedInbox_}}),
      pings_(router_.addRoute(), clock_) {}

void PlayerConnection::update(engine::Duration elapsed) {
  session_.update(elapsed);
}

void PlayerConnection::leave() {
  session_.cancel();
  while (session_.isLeaving()) {
    std::this_thread::sleep_for(DISCONNECTION_COPY_INTERVAL);
    session_.update(DISCONNECTION_COPY_INTERVAL);
  }
}

void PlayerConnection::provideParts(engine::InstallContext& context) {
  entityTypes_.drawLocalWith(context.require<EntityAppearances>());
  context.provide(session_);
  context.provide(waitingRoom_);
  context.provide(shipLooks_);
  context.provide(entityMessages_);
  context.provide(leaderboard_);
  context.provide(chat_);
  context.provide<IPlayerRoundTrips>(roundTrips_);
  context.provide<IServerTicks>(roundTrips_);
  context.provide<ISessionArrivals>(arrivals_);
  context.provide<IEntityTypeTable>(entityTypes_);
  context.provide(network_.traffic());
}

NetworkThread& PlayerConnection::network() { return network_; }

ServerSession& PlayerConnection::session() { return session_; }

const ServerSession& PlayerConnection::session() const { return session_; }

WaitingRoom& PlayerConnection::waitingRoom() { return waitingRoom_; }

const WaitingRoom& PlayerConnection::waitingRoom() const {
  return waitingRoom_;
}

EntityMessageQueue& PlayerConnection::entityMessages() {
  return entityMessages_;
}

IServerPingProbe& PlayerConnection::pings() { return pings_; }

}  // namespace rtype::client
