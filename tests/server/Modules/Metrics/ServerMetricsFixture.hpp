#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>
#include "GameInstance.hpp"
#include "IModule.hpp"
#include "MetricsReport.hpp"
#include "ServerLobbyFixture.hpp"
#include "SimulatedClock.hpp"

/** @brief The base of the tests of the metrics module: the lobby fixture with
 * the metrics module listed last, as in the server, on a clock that a probe
 * moves on by TICK_WORK_TIME in the simulation stage of each tick. */
class ServerMetricsFixture : public ServerLobbyFixture {
 protected:
  [[nodiscard]] std::vector<std::unique_ptr<rtype::engine::IModule>>
  modulesAfterTheGames() const override;

  /** @brief Starts the server with @p gameCount games. */
  void startWithGames(std::size_t gameCount);

  /** @brief Runs @p count ticks in which nothing is received. */
  void runTicks(std::size_t count);

  /** @returns How many lines of the journal the metrics wrote. */
  [[nodiscard]] std::size_t metricsLinesInTheJournal() const;

  /** @returns The report the metrics module published last, if any. */
  [[nodiscard]] std::optional<rtype::server::MetricsReport> latestReport();

  /** @brief Runs the ticks that start the game of the player at @p index,
   * seated in it and alone: it says it is ready, then its client shows the game
   * screen.
   */
  void startTheGameOfPlayer(std::size_t index);

  /** @brief Adds to @p game a system that takes TIMED_SYSTEM_TIME per run,
   * under TIMED_SYSTEM_NAME. */
  void addTimedSystemTo(rtype::server::GameInstance& game) const;

 private:
  std::shared_ptr<SimulatedClock> clock_{std::make_shared<SimulatedClock>()};
};
