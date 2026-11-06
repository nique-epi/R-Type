#include "ClientConnectionModule.hpp"
#include <exception>
#include <string>
#include <string_view>
#include <vector>
#include "ClientConstants.hpp"
#include "ConnectionConstants.hpp"
#include "EntityAppearances.hpp"
#include "IGameCatalog.hpp"
#include "IServerCatalog.hpp"
#include "InstallContext.hpp"
#include "MasterAccess.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ServerListConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

namespace {

/** @returns How the client reaches the master: its constant address and port,
 * with no key, since the reads of the list need none. */
network::MasterAccess clientMasterAccess() {
  return network::MasterAccess{
      .url = std::string{MASTER_URL_SCHEME} +
             std::string{network::LOOPBACK_IPV4_ADDRESS} +
             std::string{MASTER_PORT_SEPARATOR} +
             std::to_string(network::DEFAULT_MASTER_PORT),
      .key = std::string{}};
}

}  // namespace

ClientConnectionModule::ClientConnectionModule(
    const network::NetworkConditions& conditions)
    : connection_(conditions,
                  [this](std::string_view name) {
                    return appearances_ != nullptr &&
                           appearances_->named(name) != nullptr;
                  }),
      masterLink_(clientMasterAccess()),
      servers_(masterLink_, connection_.pings()),
      games_(masterLink_),
      logger_(std::string{CONNECTION_LOGGER_NAME}) {}

std::string ClientConnectionModule::name() const {
  return std::string{CONNECTION_MODULE_NAME};
}

std::vector<std::string> ClientConnectionModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{ASSETS_MODULE_NAME}};
}

void ClientConnectionModule::install(engine::InstallContext& context) {
  appearances_ = &context.require<EntityAppearances>();
  connection_.provideParts(context);
  context.provide<IServerCatalog>(servers_);
  context.provide<IGameCatalog>(games_);
  context.addWork(NETWORK_STAGE, [this](engine::Duration elapsed) {
    connection_.update(elapsed);
    servers_.update(elapsed);
    games_.update();
  });
}

void ClientConnectionModule::start() {
  connection_.network().open();
  try {
    masterLink_.start();
  } catch (const std::exception&) {
    connection_.network().close();
    throw;
  }
}

void ClientConnectionModule::stop() {
  masterLink_.stop();
  try {
    connection_.leave();
  } catch (const std::exception& error) {
    logger_.warn("the server was not told the client leaves: ", error.what());
  }
  connection_.network().close();
}

}  // namespace rtype::client
