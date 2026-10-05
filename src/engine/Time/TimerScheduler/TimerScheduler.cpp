#include "TimerScheduler.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>
#include <vector>
#include "EngineException.hpp"
#include "TimeConstants.hpp"
#include "TimerHandle.hpp"

namespace rtype::engine {

TimerHandle TimerScheduler::scheduleOnce(Duration delay, Callback callback) {
  if (delay < Duration::zero()) {
    throw InvalidTimerDelayException();
  }
  return add(delay, std::nullopt, std::move(callback));
}

TimerHandle TimerScheduler::scheduleRepeating(Duration interval,
                                              Callback callback) {
  if (interval <= Duration::zero()) {
    throw InvalidTimerIntervalException();
  }
  return add(interval, interval, std::move(callback));
}

bool TimerScheduler::cancel(TimerHandle handle) {
  const auto found = std::ranges::find(timers_, handle, &Timer::handle);
  if (found == timers_.end()) {
    return false;
  }
  timers_.erase(found);
  return true;
}

void TimerScheduler::advance(Duration elapsed) {
  if (elapsed < Duration::zero()) {
    throw NegativeElapsedTimeException();
  }
  const Duration target = now_ + elapsed;
  for (auto due = findEarliestDue(target); due != timers_.end();
       due = findEarliestDue(target)) {
    now_ = due->dueTime;
    const Callback callback = due->callback;
    if (due->repeatInterval.has_value()) {
      due->dueTime += *due->repeatInterval;
    } else {
      timers_.erase(due);
    }
    callback();
  }
  now_ = target;
}

std::size_t TimerScheduler::size() const { return timers_.size(); }

TimerHandle TimerScheduler::add(Duration delay,
                                std::optional<Duration> repeatInterval,
                                Callback callback) {
  const TimerHandle handle{.identifier = nextIdentifier_++};
  timers_.push_back(Timer{.handle = handle,
                          .dueTime = now_ + delay,
                          .repeatInterval = repeatInterval,
                          .callback = std::move(callback)});
  return handle;
}

std::vector<TimerScheduler::Timer>::iterator TimerScheduler::findEarliestDue(
    Duration limit) {
  const auto earliest = std::ranges::min_element(timers_, {}, &Timer::dueTime);
  if (earliest == timers_.end() || earliest->dueTime > limit) {
    return timers_.end();
  }
  return earliest;
}

}  // namespace rtype::engine
