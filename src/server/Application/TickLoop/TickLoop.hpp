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
 * MAXIMUM_TICKS_PER_ADVANCE ticks drops the excess, so the game freezes during
 * the stall instead of catching it up. Each tick is measured: how long after
 * it was due it started. After a dropped stall, the ticks run are measured
 * from when the first of them was due, so the whole stall shows. Every stall
 * that drops game time is recorded.
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
   * @brief The stalls long enough to drop game time: how many, and the game
   * time they dropped instead of catching it up.
   */
  struct Stalls {
    std::size_t count;
    engine::Duration longestDropped;
    engine::Duration totalDropped;
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
   * TICKS_PER_REPORT ticks, logs their lateness at the debug level and, when
   * stalls dropped game time since the previous report, a warning with that
   * time. Logs a warning when the system refuses a FineTimerResolution.
   *
   * It sleeps in real time, so the clock must follow real time.
   */
  void run(const std::stop_token& stop);

  /**
   * @returns How late the ticks run since the previous call started, then
   * starts measuring again.
   */
  Lateness takeLateness();

  /**
   * @returns The stalls that dropped game time since the previous call, then
   * starts recording again.
   */
  Stalls takeStalls();

 private:
  void record(engine::Duration lateness);
  void recordStall(engine::Duration droppedTime);
  void reportLateness();
  void reportStalls();

  const engine::IClock* clock_;
  engine::FixedTimestep timestep_;
  std::function<void()> tick_;
  Lateness lateness_{};
  Stalls stalls_{};
  logging::Logger logger_;
};

}  // namespace rtype::server
