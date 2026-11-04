#include "ServersModule.hpp"
#include <string>
#include <vector>
#include "ApiRequest.hpp"
#include "InstallContext.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterRouteConstants.hpp"
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
  ServersController& controller =
      controller_.emplace(context.require<MasterDatabase>());
  auto& server = context.require<MasterHttpServer>();
  server.add(network::CREATE_SERVER_ROUTE,
             [&controller](const ApiRequest& request) {
               return controller.createServer(request);
             });
  server.add(network::REPLACE_OWN_SERVER_ROUTE,
             [&controller](const ApiRequest& request) {
               return controller.replaceOwnServer(request);
             });
  server.add(network::REPLACE_OWN_HEARTBEAT_ROUTE,
             [&controller](const ApiRequest& request) {
               return controller.replaceOwnHeartbeat(request);
             });
}

void ServersModule::start() {}

void ServersModule::stop() {}

}  // namespace rtype::master
