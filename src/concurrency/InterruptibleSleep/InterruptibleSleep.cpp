#include "InterruptibleSleep.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stop_token>

namespace rtype::concurrency {

void sleepUnlessStopped(const std::stop_token& stop,
                        std::chrono::milliseconds duration) {
  std::mutex mutex;
  std::condition_variable_any neverNotified;
  std::unique_lock lock{mutex};
  static_cast<void>(
      neverNotified.wait_for(lock, stop, duration, [] { return false; }));
}

}  // namespace rtype::concurrency
