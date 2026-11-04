#include "MasterHttpModule.hpp"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ConfigurationRegistry.hpp"
#include "FailureReport.hpp"
#include "HealthResponse.hpp"
#include "HttpConstants.hpp"
#include "HttpEndpoint.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "MasterConstants.hpp"
#include "MasterModuleConstants.hpp"
#include "NetworkConstants.hpp"

namespace rtype::master {

MasterHttpModule::MasterHttpModule(FailureReport onFailure)
    : server_(std::move(onFailure)), logger_(std::string{MASTER_LOGGER_NAME}) {}

std::string MasterHttpModule::name() const {
  return std::string{HTTP_MODULE_NAME};
}

std::vector<std::string> MasterHttpModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void MasterHttpModule::install(engine::InstallContext& context) {
  portSetting_ =
      &context.require<engine::ConfigurationRegistry>().declareInteger(
          engine::IntegerSettingDefinition{
              .name = std::string{PORT_SETTING_NAME},
              .defaultValue = network::DEFAULT_MASTER_PORT,
              .lowest = network::LOWEST_PORT,
              .highest = network::HIGHEST_PORT,
              .help = "TCP port the HTTP API listens on"});
  server_.addGet(HEALTH_PATH, healthResponse);
  context.provide(server_);
}

void MasterHttpModule::start() {
  const std::uint16_t port = server_.open(HttpEndpoint{
      .address = std::string{ALL_INTERFACES}, .port = listeningPort()});
  logger_.info("HTTP API listening on TCP port ", port);
}

void MasterHttpModule::stop() { server_.close(); }

std::uint16_t MasterHttpModule::listeningPort() const {
  return static_cast<std::uint16_t>(
      portSetting_->integer().value_or(network::DEFAULT_MASTER_PORT));
}

}  // namespace rtype::master
