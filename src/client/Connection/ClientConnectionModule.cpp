#include "ClientConnectionModule.hpp"
#include <exception>
#include <string>
#include <thread>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "Endpoint.hpp"
#include "IPlayerRoundTrips.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
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

ClientConnectionModule::ClientConnectionModule(
    const network::NetworkConditions& conditions)
    : network_(anyInterfaceAnyPort(), conditions),
      router_(network_),
      waitingRoom_(dispatcher_, gameLoaded_),
      entityMessages_(dispatcher_, waitingRoom_, gameLoaded_),
      roundTrips_(dispatcher_, clock_),
      session_(ServerSessionParts{.link = router_.addRoute(),
                                  .dispatcher = dispatcher_,
                                  .gameLoaded = gameLoaded_,
                                  .observers = {waitingRoom_, entityMessages_,
                                                gameLoaded_, roundTrips_}}),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  context.provide(session_);
  context.provide(waitingRoom_);
  context.provide(entityMessages_);
  context.provide(network_.traffic());
  context.provide<IPlayerRoundTrips>(roundTrips_);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    entityMessages_.advance(elapsed);
    session_.update(elapsed);
  });
}

void ClientConnectionModule::start() { network_.open(); }

void ClientConnectionModule::stop() {
  try {
    leaveServer();
  } catch (const std::exception& error) {
    logger_.warn("the server was not told the client leaves: ", error.what());
  }
  network_.close();
}

void ClientConnectionModule::leaveServer() {
  session_.cancel();
  while (session_.isLeaving()) {
    std::this_thread::sleep_for(DISCONNECTION_COPY_INTERVAL);
    session_.update(DISCONNECTION_COPY_INTERVAL);
  }
}

}  // namespace rtype::client
