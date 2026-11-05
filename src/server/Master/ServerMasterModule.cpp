#include "ServerMasterModule.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EngineException.hpp"
#include "Games.hpp"
#include "HeartbeatJobParts.hpp"
#include "IListeningPort.hpp"
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
#include "WireTypes.hpp"

namespace rtype::server {

namespace {

void requireServerName(std::string_view name) {
  if (!network::isValidServerName(name)) {
    throw engine::CommandRefusedException(
        std::string{INVALID_SERVER_NAME_TEXT});
  }
}

}  // namespace

ServerMasterModule::ServerMasterModule()
    : logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerMasterModule::name() const {
  return std::string{MASTER_MODULE_NAME};
}

std::vector<std::string> ServerMasterModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{GAMES_MODULE_NAME}};
}

std::vector<std::string> ServerMasterModule::optionalDependencies() const {
  return {std::string{METRICS_MODULE_NAME}};
}

void ServerMasterModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  port_ = &context.require<IListeningPort>();
  games_ = &context.require<Games>();
  hostedGames_ = &context.require<LatestHostedGames>();
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
  nameSetting_ = &configuration.declareText(engine::TextSettingDefinition{
      .name = std::string{SERVER_NAME_SETTING_NAME},
      .defaultValue = std::string{DEFAULT_SERVER_NAME},
      .help = "Name players read in the list of servers",
      .validate = requireServerName});
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
      .identity = ServerIdentity{.name = nameSetting_->text(),
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
