#include "ServerMetricsFixture.hpp"
#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ClockAdvancingSystem.hpp"
#include "ConnectedPlayer.hpp"
#include "GameInstance.hpp"
#include "IModule.hpp"
#include "JournalFile.hpp"
#include "LatestMetrics.hpp"
#include "MetricsReport.hpp"
#include "ModuleConstants.hpp"
#include "PlayerRoster.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyFixture.hpp"
#include "ServerMetricsModule.hpp"
#include "ServerMetricsTestConstants.hpp"
#include "ServiceProbeModule.hpp"
#include "SystemScheduler.hpp"
#include "TimeConstants.hpp"
#include "TrafficTestConstants.hpp"

std::vector<std::unique_ptr<rtype::engine::IModule>>
ServerMetricsFixture::modulesAfterTheGames() const {
  std::vector<std::unique_ptr<rtype::engine::IModule>> modules =
      ServerLobbyFixture::modulesAfterTheGames();
  modules.push_back(std::make_unique<rtype::server::ServerMetricsModule>(
      clock_, trafficFile_));
  modules.push_back(std::make_unique<ServiceProbeModule>(
      METRICS_CLOCK_PROBE_MODULE,
      std::vector<std::string>{std::string{rtype::server::GAMES_MODULE_NAME}},
      [clock = clock_]([[maybe_unused]] rtype::engine::Duration elapsed) {
        clock->advance(TICK_WORK_TIME);
      }));
  return modules;
}

void ServerMetricsFixture::runTicks(std::size_t count) {
  for (std::size_t tick = 0; tick < count; ++tick) {
    runTick({});
  }
}

std::size_t ServerMetricsFixture::metricsLinesInTheJournal() const {
  return journal().linesHolding(METRICS_LINE_MARK).size();
}

std::size_t ServerMetricsFixture::trafficLinesInTheJournal() const {
  return journal().linesHolding(TRAFFIC_LINE_MARK).size();
}

void ServerMetricsFixture::writeTrafficTo(std::filesystem::path file) {
  trafficFile_ = std::move(file);
}

std::optional<rtype::server::MetricsReport>
ServerMetricsFixture::latestReport() {
  return probe().latestMetrics()->value();
}

void ServerMetricsFixture::startTheGameOfPlayer(std::size_t index) {
  sayReady(index);
  rtype::server::ConnectedPlayer* const seated =
      probe().runningGame()->game().roster().find(
          rtype::server::testing::playerEndpoint(index));
  if (seated != nullptr) {
    seated->loaded = true;
  }
  runTick({});
}

void ServerMetricsFixture::addTimedSystemTo(
    rtype::server::GameInstance& game) const {
  game.systems().add(
      std::make_unique<ClockAdvancingSystem>(*clock_, TIMED_SYSTEM_TIME),
      TIMED_SYSTEM_NAME);
}
