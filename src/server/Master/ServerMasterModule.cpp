#include "ServerMasterModule.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "Games.hpp"
#include "HeartbeatJobParts.hpp"
#include "IListeningPort.hpp"
#include "IServerName.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "LatestMetrics.hpp"
#include "MasterAccess.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerIdentity.hpp"
#include "ServerMasterConstants.hpp"
#include "TextSettingDefinition.hpp"

namespace rtype::server {

ServerMasterModule::ServerMasterModule()
    : logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerMasterModule::name() const {
  return std::string{MASTER_MODULE_NAME};
}

std::vector<std::string> ServerMasterModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{GAMES_MODULE_NAME}, std::string{INFORMATION_MODULE_NAME}};
}

std::vector<std::string> ServerMasterModule::optionalDependencies() const {
  return {std::string{METRICS_MODULE_NAME}};
}

void ServerMasterModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  port_ = &context.require<IListeningPort>();
  games_ = &context.require<Games>();
  hostedGames_ = &context.require<LatestHostedGames>();
  serverName_ = &context.require<IServerName>();
  metrics_ = context.find<LatestMetrics>();
}

void ServerMasterModule::declareSettings(
    engine::ConfigurationRegistry& configuration) {
  urlSetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{MASTER_URL_SETTING_NAME},
      .defaultValue = {},
      .help = "http://<host>[:<port>] of the master; empty to run alone",
      .validate = {}});
  keySetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{MASTER_KEY_SETTING_NAME},
      .defaultValue = {},
      .help = "Key the master gave when this server was created",
      .validate = {}});
}

std::optional<HeartbeatJobParts> ServerMasterModule::jobParts() const {
  if (urlSetting_->text().empty() || keySetting_->text().empty()) {
    logger_.info("no master-url or master-key: the server runs alone");
    return std::nullopt;
  }
  const std::uint16_t udpPort = port_->listeningPort();
  if (udpPort == network::ANY_PORT) {
    logger_.warn("the port is chosen by the system: the master is not told");
    return std::nullopt;
  }
  return HeartbeatJobParts{
      .access = network::MasterAccess{.url = urlSetting_->text(),
                                      .key = keySetting_->text()},
      .identity = ServerIdentity{.name = serverName_->serverName(),
                                 .udpPort = udpPort,
                                 .maximumGames = static_cast<std::uint16_t>(
                                     games_->all().size())},
      .games = *hostedGames_,
      .metrics = metrics_,
      .times = {}};
}

void ServerMasterModule::start() {
  std::optional<HeartbeatJobParts> parts = jobParts();
  if (!parts.has_value()) {
    return;
  }
  try {
    job_.emplace(std::move(*parts));
  } catch (const network::InvalidMasterUrlException& invalid) {
    logger_.error(invalid.what(), ": the server runs alone");
  }
}

void ServerMasterModule::stop() { job_.reset(); }

}  // namespace rtype::server
