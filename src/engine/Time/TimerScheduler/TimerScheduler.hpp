#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
#include "TimeConstants.hpp"
#include "TimerHandle.hpp"

namespace rtype::engine {

/**
 * @brief Runs callbacks after a delay or at a regular interval, measured in
 * simulation time.
 *
 * The scheduler never reads a clock: the caller feeds it the elapsed
 * simulation time through advance(), typically SIMULATION_TICK_DURATION once
 * per tick. Timers due at the same instant fire in the order they were
 * created. A repeating timer fires once per elapsed interval, even when
 * several intervals pass in a single advance(). A callback may schedule or
 * cancel timers, itself included; a timer scheduled from a callback starts
 * at the due time of the timer that is firing.
 */
class TimerScheduler {
 public:
  using Callback = std::function<void()>;

  /**
   * @throws InvalidTimerDelayException if delay is negative.
   */
  TimerHandle scheduleOnce(Duration delay, Callback callback);

  /**
   * @throws InvalidTimerIntervalException if interval is not positive.
   */
  TimerHandle scheduleRepeating(Duration interval, Callback callback);

  /**
   * @return true if a pending timer was cancelled, false if the handle is
   * unknown, already fired (one-shot) or already cancelled.
   */
  bool cancel(TimerHandle handle);

  /**
   * @throws NegativeElapsedTimeException if elapsed is negative.
   */
  void advance(Duration elapsed);

  /** @return number of pending timers. */
  std::size_t size() const;

 private:
  struct Timer {
    TimerHandle handle;
    Duration dueTime;
    std::optional<Duration> repeatInterval;
    Callback callback;
  };

  TimerHandle add(Duration delay, std::optional<Duration> repeatInterval,
                  Callback callback);
  std::vector<Timer>::iterator findEarliestDue(Duration limit);

  std::vector<Timer> timers_;
  Duration now_{};
  std::uint64_t nextIdentifier_{0};
};

}  // namespace rtype::engine
