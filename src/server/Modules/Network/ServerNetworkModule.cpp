#include "ServerNetworkModule.hpp"
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include "Application.hpp"
#include "Endpoint.hpp"
#include "IDatagramSender.hpp"
#include "IPlayerAdmission.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
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

ServerNetworkModule::ServerNetworkModule(std::uint16_t port,
                                         StopRequest& stopRequest)
    : port_(port),
      stopRequest_(&stopRequest),
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
      });
  context.provide<IDatagramSender>(*server_);
  context.provide<IPlayerAdmission>(*server_);
  context.provide(tickDatagrams_);
}

void ServerNetworkModule::start() {
  server_->open();
  try {
    serverThread_ = std::jthread{[this] { runServer(); }};
  } catch (...) {
    server_->close();
    throw;
  }
  logger_.info("listening on UDP port ", server_->localEndpoint().port);
}

void ServerNetworkModule::stop() {
  server_->stop();
  serverThread_.join();
  server_->close();
}

void ServerNetworkModule::runServer() {
  try {
    server_->run();
  } catch (const std::exception& error) {
    stopRequest_->fail(error.what());
  }
}

}  // namespace rtype::server
