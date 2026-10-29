#include "ClientConnectionModule.hpp"
#include <exception>
#include <string>
#include <thread>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "Endpoint.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns Every IPv4 interface, on the port the system picks. */
network::Endpoint anyInterfaceAnyPort() {
  return network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                           .port = network::ANY_PORT};
}

}  // namespace

ClientConnectionModule::ClientConnectionModule()
    : network_(anyInterfaceAnyPort()),
      connection_(network_),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  context.provide(connection_);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
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
  connection_.cancel();
  while (connection_.isLeaving()) {
    std::this_thread::sleep_for(DISCONNECTION_COPY_INTERVAL);
    connection_.update(DISCONNECTION_COPY_INTERVAL);
  }
}

}  // namespace rtype::client
