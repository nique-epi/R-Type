#include "ClientConnectionModule.hpp"
#include <exception>
#include <string>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "IPlayerRoundTrips.hpp"
#include "IServerTicks.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ClientConnectionModule::ClientConnectionModule(
    const network::NetworkConditions& conditions)
    : connection_(conditions), logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  context.provide(connection_.session());
  context.provide(connection_.waitingRoom());
  context.provide(connection_.entityMessages());
  context.provide(connection_.leaderboard());
  context.provide<IPlayerRoundTrips>(connection_.roundTrips());
  context.provide<IServerTicks>(connection_.roundTrips());
  context.provide(connection_.network().traffic());
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
  });
}

void ClientConnectionModule::start() { connection_.network().open(); }

void ClientConnectionModule::stop() {
  try {
    connection_.leave();
  } catch (const std::exception& error) {
    logger_.warn("the server was not told the client leaves: ", error.what());
  }
  connection_.network().close();
}

}  // namespace rtype::client
