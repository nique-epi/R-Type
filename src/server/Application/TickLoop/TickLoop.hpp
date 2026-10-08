#pragma once

#include <cstddef>
#include <functional>
#include <stop_token>
#include "FixedTimestep.hpp"
#include "IClock.hpp"
#include "Logger.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief Calls a function once per simulation tick, SIMULATION_TICKS_PER_SECOND
 * times per second, on the thread that runs it.
 *
 * A FixedTimestep counts the ticks on the given clock: after a late wake-up the
 * missed ticks run back to back, and a stall longer than
 * MAXIMUM_TICKS_PER_ADVANCE ticks drops the excess. Each tick is measured: how
 * long after it was due it started.
 */
class TickLoop {
 public:
  /**
   * @brief How late a series of ticks started, each compared with the moment
   * it was due.
   */
  struct Lateness {
    std::size_t tickCount;
    engine::Duration maximum;
    engine::Duration total;
  };

  /**
   * @param clock Read for every tick; it must outlive the loop.
   * @param tick Called once per tick; it must not be empty.
   */
  TickLoop(const engine::IClock& clock, std::function<void()> tick);

  /**
   * @brief Runs every tick due since the previous call, or since the
   * construction, then returns without waiting.
   * @returns The number of ticks run.
   */
  std::size_t runDueTicks();

  /**
   * @brief Runs the due ticks, then sleeps until the next one is due, until
   * @p stop is requested; a request is seen when the current sleep ends. Every
   * TICKS_PER_LATENESS_REPORT ticks, logs their lateness at the debug level.
   * Logs a warning when the system refuses a FineTimerResolution.
   *
   * It sleeps in real time, so the clock must follow real time.
   */
  void run(const std::stop_token& stop);

  /**
   * @returns How late the ticks run since the previous call started, then
   * starts measuring again.
   */
  Lateness takeLateness();

 private:
  void record(engine::Duration lateness);
  void reportLateness();

  const engine::IClock* clock_;
  engine::FixedTimestep timestep_;
  std::function<void()> tick_;
  engine::Duration nextTickDue_;
  Lateness lateness_{};
  logging::Logger logger_;
};

}  // namespace rtype::server
