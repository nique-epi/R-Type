#include "ServerNetworkModule.hpp"
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "Application.hpp"
#include "ConfigurationRegistry.hpp"
#include "Endpoint.hpp"
#include "IDatagramSender.hpp"
#include "IListeningPort.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "IntegerSettingDefinition.hpp"
#include "ModuleConstants.hpp"
#include "NetworkConditions.hpp"
#include "NetworkConstants.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "StopRequest.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The address and port the server listens on: every IPv4 interface,
 * on @p port.
 */
network::Endpoint listeningEndpoint(std::uint16_t port) {
  return network::Endpoint{.address = std::string{network::ANY_IPV4_ADDRESS},
                           .port = port};
}

}  // namespace

ServerNetworkModule::ServerNetworkModule(
    std::uint16_t port, StopRequest& stopRequest,
    const network::NetworkConditions& conditions)
    : port_(port),
      stopRequest_(&stopRequest),
      conditions_(conditions),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string ServerNetworkModule::name() const {
  return std::string{NETWORK_MODULE_NAME};
}

std::vector<std::string> ServerNetworkModule::dependencies() const {
  return {std::string{LOGGER_MODULE_NAME}};
}

void ServerNetworkModule::install(engine::InstallContext& context) {
  auto& application = context.require<engine::Application>();
  server_ = std::make_unique<ServerApplication>(
      listeningEndpoint(port_),
      [this,
       &application](const ServerApplication::ReceivedDatagrams& received) {
        tickDatagrams_.begin(received);
        application.runStages(engine::SIMULATION_TICK_DURATION);
        tickDatagrams_.end();
      },
      conditions_);
  portSetting_ =
      &context.require<engine::ConfigurationRegistry>().declareInteger(
          engine::IntegerSettingDefinition{
              .name = std::string{PORT_SETTING_NAME},
              .defaultValue = port_,
              .lowest = network::LOWEST_PORT,
              .highest = network::HIGHEST_PORT,
              .help = "UDP port to listen on"});
  context.provide<IDatagramSender>(*server_);
  context.provide<IPlayerAdmission>(*server_);
  context.provide(tickDatagrams_);
  context.provide<IListeningPort>(*this);
  context.provide(server_->traffic());
}

void ServerNetworkModule::start() {
  server_->listenOn(listeningEndpoint(listeningPort()));
  const network::Endpoint bound = server_->open();
  try {
    serverThread_ = std::jthread{[this] { runServer(); }};
  } catch (...) {
    server_->close();
    throw;
  }
  logger_.info("listening on UDP port ", bound.port);
}

void ServerNetworkModule::stop() {
  server_->stop();
  serverThread_.join();
  server_->close();
}

std::uint16_t ServerNetworkModule::listeningPort() const {
  return static_cast<std::uint16_t>(portSetting_->integer().value_or(port_));
}

void ServerNetworkModule::runServer() {
  try {
    server_->run();
  } catch (const std::exception& error) {
    stopRequest_->fail(error.what());
  }
}

}  // namespace rtype::server
