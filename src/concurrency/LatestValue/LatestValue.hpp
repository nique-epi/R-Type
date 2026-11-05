#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <stop_token>
#include <utility>

namespace rtype::concurrency {

/**
 * @brief The newest value one thread publishes for others to read, as a watch
 * channel keeps it: a reader gets the latest value, never a queue of them, and
 * may sleep until a newer one comes.
 *
 * Every call is safe from any thread. Reading changes nothing, so readers at
 * different rates all see the same value. The lock is held for a move or a
 * copy only, so a slow reader never slows the thread that publishes.
 */
template <typename Value>
class LatestValue {
 public:
  /** @brief Replaces the value held by @p value, and wakes waitForNewer(). */
  void publish(Value value) {
    {
      const std::scoped_lock lock{mutex_};
      value_ = std::move(value);
      ++revision_;
    }
    published_.notify_all();
  }

  /** @returns A copy of the newest value, or nothing before the first one. */
  [[nodiscard]] std::optional<Value> value() const {
    const std::scoped_lock lock{mutex_};
    return value_;
  }

  /** @returns How many values were published: 0 before the first one. */
  [[nodiscard]] std::uint64_t revision() const {
    const std::scoped_lock lock{mutex_};
    return revision_;
  }

  /**
   * @brief Sleeps until a value newer than revision @p seen is published, @p
   * timeout passed, or @p stop is requested, whichever comes first.
   * @returns The revision held when it wakes: above @p seen when a newer value
   * came.
   */
  std::uint64_t waitForNewer(std::uint64_t seen,
                             std::chrono::steady_clock::duration timeout,
                             const std::stop_token& stop) const {
    std::unique_lock lock{mutex_};
    published_.wait_for(lock, stop, timeout,
                        [this, seen] { return revision_ > seen; });
    return revision_;
  }

 private:
  mutable std::mutex mutex_;
  mutable std::condition_variable_any published_;
  std::optional<Value> value_;
  std::uint64_t revision_{0};
};

}  // namespace rtype::concurrency
