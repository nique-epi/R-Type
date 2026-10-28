#include "NetworkModule.hpp"
#include <string>
#include <utility>
#include <vector>
#include "Endpoint.hpp"
#include "IServerLink.hpp"
#include "IStageRunner.hpp"
#include "InstallContext.hpp"
#include "ServerApplication.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

NetworkModule::NetworkModule(network::Endpoint localEndpoint)
    : localEndpoint_(std::move(localEndpoint)),
      logger_(std::string{SERVER_LOGGER_NAME}) {}

std::string NetworkModule::name() const {
  return std::string{NETWORK_MODULE_NAME};
}

std::vector<std::string> NetworkModule::dependencies() const {
  return {std::string{JOURNAL_MODULE_NAME}};
}

void NetworkModule::install(engine::InstallContext& context) {
  stages_ = &context.require<engine::IStageRunner>();
  context.provide<IServerLink>(link_);
  context.provide(received_);
}

void NetworkModule::start() {
  link_.open(localEndpoint_,
             [this](const ServerApplication::ReceivedDatagrams& received) {
               runTick(received);
             });
  try {
    stopSignals_.emplace();
  } catch (...) {
    link_.close();
    throw;
  }
}

void NetworkModule::stop() {
  stopSignals_.reset();
  link_.close();
}

void NetworkModule::run() {
  logger_.info("listening on UDP port ", link_.localEndpoint().port);
  link_.run();
}

void NetworkModule::runTick(
    const ServerApplication::ReceivedDatagrams& received) {
  received_ = received;
  stages_->runStages(engine::SIMULATION_TICK_DURATION);
  received_ = ServerApplication::ReceivedDatagrams{};
  if (stopSignals_.has_value() && stopSignals_->received()) {
    link_.stop();
  }
}

}  // namespace rtype::server
