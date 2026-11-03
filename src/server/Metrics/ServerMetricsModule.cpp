#include "ServerMetricsModule.hpp"
#include <memory>
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

namespace rtype::server {

ServerMetricsModule::ServerMetricsModule(
    std::shared_ptr<const engine::IClock> clock)
    : clock_(std::move(clock)), logger_(std::string{METRICS_LOGGER_NAME}) {}

std::string ServerMetricsModule::name() const {
  return std::string{METRICS_MODULE_NAME};
}

std::vector<std::string> ServerMetricsModule::dependencies() const {
  return {std::string{GAMES_MODULE_NAME}};
}

void ServerMetricsModule::install(engine::InstallContext& context) {
  games_ = &context.require<Games>();
  context.provide(latest_);
  context.addWork(INPUT_STAGE,
                  [this]([[maybe_unused]] engine::Duration elapsed) {
                    tickStart_ = clock_->now();
                  });
  context.addWork(
      OUTPUT_STAGE,
      [this]([[maybe_unused]] engine::Duration elapsed) { endTick(); });
}

void ServerMetricsModule::start() { measureSystemsOf(*games_, clock_.get()); }

void ServerMetricsModule::stop() { measureSystemsOf(*games_, nullptr); }

void ServerMetricsModule::endTick() {
  durations_.record(clock_->now() - tickStart_);
  if (durations_.isFull()) {
    reportTheTicks();
  }
}

void ServerMetricsModule::reportTheTicks() {
  MetricsReport report{.ticks = durations_.takeSummary(),
                       .games = censusOf(*games_),
                       .systemTimes = takeSystemTimesOf(*games_)};
  logger_.info(ticksLineOf(report));
  logger_.info(systemsLineOf(report));
  latest_.publish(std::move(report));
}

}  // namespace rtype::server
