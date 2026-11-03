#pragma once

#include <memory>
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
 * the server runs the same without this module.
 *
 * start() has the schedulers of the games read its clock, which exist only once
 * the games module started; stop() takes the clock back. The games are never
 * replaced, so telling them once is enough. Everything runs on the simulation
 * thread, and nothing allocates per tick.
 */
class ServerMetricsModule final : public engine::IModule {
 public:
  /** @param clock Read at both ends of each tick and around each system. */
  explicit ServerMetricsModule(std::shared_ptr<const engine::IClock> clock);

  [[nodiscard]] std::string name() const override;
  [[nodiscard]] std::vector<std::string> dependencies() const override;

  /**
   * @throws MissingServiceException when no module provided the games.
   */
  void install(engine::InstallContext& context) override;

  /** @brief Starts measuring the systems of every game. */
  void start() override;

  /** @brief Stops measuring the systems of every game. */
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
};

}  // namespace rtype::server
