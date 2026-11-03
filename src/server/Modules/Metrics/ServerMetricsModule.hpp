#pragma once

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Games.hpp"
#include "IClock.hpp"
#include "IModule.hpp"
#include "InstallContext.hpp"
#include "LatestMetrics.hpp"
#include "Logger.hpp"
#include "TickDurations.hpp"
#include "TimeConstants.hpp"
#include "TrafficCounters.hpp"
#include "TrafficRecorder.hpp"

namespace rtype::server {

/**
 * @brief The module that measures the server and reports it every
 * TICKS_PER_REPORT ticks: how long the ticks last, what each system of the
 * games costs, and how many games, players and entities there are.
 *
 * Its work brackets the stages of a tick: its work in the inputs stage, which
 * no other module uses, starts the measure, and its work in the outputs stage
 * ends it. It is listed last, so that second work comes after the work of every
 * other module. What a tick does around the stages (emptying the queues of
 * datagrams, handing over what to send) is not counted.
 *
 * Each report goes to the journal at the info level and is published as
 * LatestMetrics, which the module provides. No module requires that service, so
 * the server runs the same without this module. The module also writes the
 * traffic of the socket as time goes by (see network::TrafficRecorder).
 *
 * start() has the schedulers of the games read its clock, which exist only once
 * the games module started; stop() takes the clock back. The games are never
 * replaced, so telling them once is enough. Everything runs on the simulation
 * thread, and a tick allocates nothing unless a report or the traffic is due.
 */
class ServerMetricsModule final : public engine::IModule {
 public:
  /** @param clock Read at both ends of each tick and around each system.
   * @param trafficFile Where the rows of the traffic go, see
   * network::TrafficRecorder; nothing writes no rows. */
  ServerMetricsModule(std::shared_ptr<const engine::IClock> clock,
                      std::optional<std::filesystem::path> trafficFile);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the games or the
   * traffic.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Starts writing the traffic and measuring the systems of every
   * game.
   * @throws network::TrafficFileOpenException when the file cannot be opened.
   */
  void start() override;

  /** @brief Stops measuring the systems of every game, and writes the last rows
   * of the traffic. */
  void stop() override;

 private:
  void endTick();
  void reportTheTicks();

  std::shared_ptr<const engine::IClock> clock_;
  logging::Logger logger_;
  Games* games_{nullptr};
  TickDurations durations_;
  LatestMetrics latest_;
  engine::Duration tickStart_{};
  std::optional<std::filesystem::path> trafficFile_;
  const network::TrafficCounters* traffic_{nullptr};
  std::unique_ptr<network::TrafficRecorder> trafficRecorder_;
};

}  // namespace rtype::server
