#include "ServersModule.hpp"
#include <functional>
#include <string>
#include <vector>
#include "InstallContext.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "ServerStatusJobConstants.hpp"
#include "ServersController.hpp"

namespace rtype::master {

std::string ServersModule::name() const {
  return std::string{SERVERS_MODULE_NAME};
}

std::vector<std::string> ServersModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{HTTP_MODULE_NAME}};
}

void ServersModule::install(engine::InstallContext& context) {
  database_ = &context.require<MasterDatabase>();
  ServersController& controller = controller_.emplace(*database_);
  auto& server = context.require<MasterHttpServer>();
  server.add(network::CREATE_SERVER_ROUTE,
             std::bind_front(&ServersController::createServer, &controller));
  server.add(
      network::REPLACE_OWN_SERVER_ROUTE,
      std::bind_front(&ServersController::replaceOwnServer, &controller));
  server.add(
      network::REPLACE_OWN_HEARTBEAT_ROUTE,
      std::bind_front(&ServersController::replaceOwnHeartbeat, &controller));
  server.add(network::LIST_SERVER_GAMES_ROUTE,
             std::bind_front(&ServersController::listGames, &controller));
  server.add(network::LIST_SERVERS_ROUTE,
             std::bind_front(&ServersController::listServers, &controller));
  server.add(network::LIST_SERVER_SAMPLES_ROUTE,
             std::bind_front(&ServersController::listSamples, &controller));
  server.add(
      network::LIST_SERVER_STATUS_CHANGES_ROUTE,
      std::bind_front(&ServersController::listStatusChanges, &controller));
}

void ServersModule::start() {
  statusJob_.emplace(*database_, STATUS_REFRESH_INTERVAL);
}

void ServersModule::stop() { statusJob_.reset(); }

}  // namespace rtype::master
