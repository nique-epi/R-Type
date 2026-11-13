#include "ServersModule.hpp"
#include <functional>
#include <string>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "IProbeSender.hpp"
#include "InstallContext.hpp"
#include "MasterConstants.hpp"
#include "MasterDatabase.hpp"
#include "MasterHttpServer.hpp"
#include "MasterModuleConstants.hpp"
#include "MasterRouteConstants.hpp"
#include "ServerStatusJobConstants.hpp"
#include "ServersController.hpp"
#include "ServersService.hpp"
#include "SettingValue.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::master {

namespace {

/** @returns The `site-key` setting, declared in @p configuration. It has no
 * check: a refused line would write its value in the journal. */
const engine::SettingValue& declareSiteKey(
    engine::ConfigurationRegistry& configuration) {
  return configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{SITE_KEY_SETTING_NAME},
      .defaultValue = {},
      .help = "Secret the web site sends in X-Site-Key, so a creation is "
              "counted under the visitor it names in X-Site-Client-Address; "
              "empty to count every creation under the address it comes from",
      .validate = {}});
}

}  // namespace

std::string ServersModule::name() const {
  return std::string{SERVERS_MODULE_NAME};
}

std::vector<std::string> ServersModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{STORAGE_MODULE_NAME},
          std::string{PROBE_MODULE_NAME}, std::string{HTTP_MODULE_NAME}};
}

void ServersModule::install(engine::InstallContext& context) {
  database_ = &context.require<MasterDatabase>();
  ServersService& service = service_.emplace(*database_);
  context.provide(service);
  ServersController& controller = controller_.emplace(
      service, context.require<IProbeSender>(),
      declareSiteKey(context.require<engine::ConfigurationRegistry>()));
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
  server.add(network::LIST_SERVER_STATUSES_ROUTE,
             std::bind_front(&ServersController::listStatuses, &controller));
}

void ServersModule::start() {
  statusJob_.emplace(*database_, STATUS_REFRESH_INTERVAL);
}

void ServersModule::stop() { statusJob_.reset(); }

}  // namespace rtype::master
