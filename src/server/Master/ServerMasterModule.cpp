#include "ServerMasterModule.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "EndpointText.hpp"
#include "EngineException.hpp"
#include "HeartbeatJobParts.hpp"
#include "IListeningPort.hpp"
#include "IServerName.hpp"
#include "IncomingDatagram.hpp"
#include "InstallContext.hpp"
#include "LatestHostedGames.hpp"
#include "LatestServerMeasures.hpp"
#include "MasterAccess.hpp"
#include "MasterMessages.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConstants.hpp"
#include "NetworkException.hpp"
#include "ServerConstants.hpp"
#include "ServerIdentity.hpp"
#include "TextSettingDefinition.hpp"
#include "TickDatagrams.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Accepts an empty public-address, which declares none, or an IPv4
 * address written as four numbers.
 * @throws engine::CommandRefusedException for any other text. */
void requirePublicAddress(std::string_view address) {
  if (address.empty()) {
    return;
  }
  try {
    static_cast<void>(network::ipv4AddressFromText(address));
  } catch (const network::NetworkException& error) {
    throw engine::CommandRefusedException(error.what());
  }
}

/** @returns The address @p text declares, nothing when it is empty; the
 * setting checked it with requirePublicAddress(). */
std::optional<std::string> declaredAddressOf(const std::string& text) {
  if (text.empty()) {
    return std::nullopt;
  }
  return text;
}

}  // namespace

ServerMasterModule::ServerMasterModule()
    : probeSecret_(randomProbeSecret()),
      probeDesk_(probeSecret_, probeTokens_),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerMasterModule::name() const {
  return std::string{MASTER_MODULE_NAME};
}

std::vector<std::string> ServerMasterModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}, std::string{NETWORK_MODULE_NAME},
          std::string{SUPERVISED_GAMES_MODULE_NAME},
          std::string{INFORMATION_MODULE_NAME}};
}

void ServerMasterModule::install(engine::InstallContext& context) {
  declareSettings(context.require<engine::ConfigurationRegistry>());
  port_ = &context.require<IListeningPort>();
  hostedGames_ = &context.require<LatestHostedGames>();
  measures_ = &context.require<LatestServerMeasures>();
  serverName_ = &context.require<IServerName>();
  tickDatagrams_ = &context.require<TickDatagrams>();
  context.addWork(
      SIMULATION_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { keepProbeTokens(); });
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
  publicAddressSetting_ =
      &configuration.declareText(engine::TextSettingDefinition{
          .name = std::string{PUBLIC_ADDRESS_SETTING_NAME},
          .defaultValue = {},
          .help = "IPv4 address players reach this server at; empty to let "
                  "the master take the address it is called from",
          .validate = requirePublicAddress});
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
      .identity =
          ServerIdentity{
              .name = serverName_->serverName(),
              .udpPort = udpPort,
              .maximumGames = static_cast<std::uint16_t>(MAXIMUM_RUNNING_GAMES),
              .probeSecret = probeSecret_,
              .address = declaredAddressOf(publicAddressSetting_->text())},
      .games = *hostedGames_,
      .probeTokens = probeTokens_,
      .measures = measures_,
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

void ServerMasterModule::keepProbeTokens() {
  const std::span<const network::IncomingDatagram> probes =
      tickDatagrams_->probes();
  if (!probes.empty()) {
    probeDesk_.keep(probes, clock_.now());
  }
}

}  // namespace rtype::server
