#include "ServerMetricsModule.hpp"
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "Games.hpp"
#include "GamesMeasures.hpp"
#include "IClock.hpp"
#include "InstallContext.hpp"
#include "MetricsConstants.hpp"
#include "MetricsLines.hpp"
#include "MetricsReport.hpp"
#include "ModuleConstants.hpp"
#include "ServerConstants.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficRecorder.hpp"

namespace rtype::server {

ServerMetricsModule::ServerMetricsModule(
    std::shared_ptr<const engine::IClock> clock,
    std::optional<std::filesystem::path> trafficFile)
    : clock_(std::move(clock)),
      logger_(std::string{METRICS_LOGGER_NAME}),
      trafficFile_(std::move(trafficFile)) {}

std::string ServerMetricsModule::name() const {
  return std::string{METRICS_MODULE_NAME};
}

std::vector<std::string> ServerMetricsModule::dependencies() const {
  return {std::string{GAMES_MODULE_NAME}, std::string{NETWORK_MODULE_NAME}};
}

void ServerMetricsModule::install(engine::InstallContext& context) {
  games_ = &context.require<Games>();
  traffic_ = &context.require<network::TrafficCounters>();
  context.provide(latest_);
  context.addWork(INPUT_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    tickStart_ = clock_->now();
                  });
  context.addWork(
      OUTPUT_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { endTick(); });
}

void ServerMetricsModule::start() {
  trafficRecorder_ = std::make_unique<network::TrafficRecorder>(
      *traffic_, trafficFile_, clock_->now());
  measureSystemsOf(*games_, clock_.get());
}

void ServerMetricsModule::stop() {
  measureSystemsOf(*games_, nullptr);
  trafficRecorder_->finish(clock_->now());
  trafficRecorder_.reset();
}

void ServerMetricsModule::endTick() {
  const engine::Duration now = clock_->now();
  durations_.record(now - tickStart_);
  if (durations_.isFull()) {
    reportTheTicks();
  }
  trafficRecorder_->advance(now);
}

void ServerMetricsModule::reportTheTicks() {
  MetricsReport report{.ticks = durations_.takeSummary(),
                       .games = censusOf(*games_),
                       .systemTimes = takeSystemTimesOf(*games_),
                       .traffic = traffic_->report(),
                       .roundTrips = roundTripsOf(*games_)};
  logger_.info(ticksLineOf(report));
  logger_.info(systemsLineOf(report));
  logger_.info(roundTripsLineOf(report));
  latest_.publish(std::move(report));
}

}  // namespace rtype::server
