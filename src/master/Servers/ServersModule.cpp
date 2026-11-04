#include "ServersModule.hpp"
#include <string>
#include <vector>
#include "ApiRequest.hpp"
#include "InstallContext.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "ServerKeys.hpp"
#include "ServersConstants.hpp"
#include "ServersController.hpp"

namespace rtype::master {

std::string ServersModule::name() const {
  return std::string{SERVERS_MODULE_NAME};
}

std::vector<std::string> ServersModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{SERVER_KEYS_MODULE_NAME}, std::string{HTTP_MODULE_NAME}};
}

void ServersModule::install(engine::InstallContext& context) {
  ServersController& controller = controller_.emplace(
      context.require<ServerKeys>(), context.require<MasterDatabase>());
  auto& server = context.require<MasterHttpServer>();
  server.addPut(OWN_SERVER_PATH, [&controller](const ApiRequest& request) {
    return controller.registration(request);
  });
  server.addPut(OWN_HEARTBEAT_PATH, [&controller](const ApiRequest& request) {
    return controller.heartbeat(request);
  });
}

void ServersModule::start() {}

void ServersModule::stop() {}

}  // namespace rtype::master
