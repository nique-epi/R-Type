#include "PlayerConnection.hpp"
#include <string>
#include <thread>
#include "ConnectionConstants.hpp"
#include "Endpoint.hpp"
#include "IPlayerRoundTrips.hpp"
#include "InstallContext.hpp"
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

PlayerConnection::PlayerConnection(const network::NetworkConditions& conditions)
    : network_(anyInterfaceAnyPort(), conditions),
      router_(network_),
      waitingRoom_(dispatcher_, gameLoaded_),
      entityMessages_(dispatcher_, waitingRoom_, gameLoaded_),
      roundTrips_(dispatcher_, clock_),
      leaderboard_(dispatcher_),
      session_(ServerSessionParts{
          .link = router_.addRoute(),
          .dispatcher = dispatcher_,
          .gameLoaded = gameLoaded_,
          .observers = {waitingRoom_, entityMessages_, gameLoaded_, roundTrips_,
                        leaderboard_}}) {}

void PlayerConnection::update(engine::Duration elapsed) {
  entityMessages_.advance(elapsed);
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
  context.provide(session_);
  context.provide(waitingRoom_);
  context.provide(entityMessages_);
  context.provide(leaderboard_);
  context.provide<IPlayerRoundTrips>(roundTrips_);
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

}  // namespace rtype::client
