#include "ClientMetricsModule.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ClientConstants.hpp"
#include "IClock.hpp"
#include "InstallContext.hpp"
#include "ModuleConstants.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficRecorder.hpp"

namespace rtype::client {

ClientMetricsModule::ClientMetricsModule(
    std::shared_ptr<const engine::IClock> clock,
    std::optional<std::filesystem::path> trafficFile)
    : clock_(std::move(clock)), trafficFile_(std::move(trafficFile)) {}

std::string ClientMetricsModule::name() const {
  return std::string{METRICS_MODULE_NAME};
}

std::vector<std::string> ClientMetricsModule::dependencies() const {
  return {std::string{CONNECTION_MODULE_NAME}};
}

void ClientMetricsModule::install(engine::InstallContext& context) {
  traffic_ = &context.require<network::TrafficCounters>();
  context.addWork(NETWORK_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    recorder_->advance(clock_->now());
                  });
}

void ClientMetricsModule::start() {
  recorder_ = std::make_unique<network::TrafficRecorder>(
      *traffic_, trafficFile_, clock_->now());
}

void ClientMetricsModule::stop() {
  recorder_->finish(clock_->now());
  recorder_.reset();
}

}  // namespace rtype::client
