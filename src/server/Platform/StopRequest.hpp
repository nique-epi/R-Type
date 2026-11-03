#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <string>
#include "StopReason.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

/**
 * @brief The place where the thread that runs the server waits to be told to
 * stop, and where whatever asks for the stop says so: the system on Ctrl-C or
 * SIGTERM (see StopSignals), or a part of the server that failed.
 *
 * Only the first request counts: a failure that follows a signal does not
 * replace it. Every function is safe from any thread.
 */
class StopRequest {
 public:
  /** @brief Asks the server to stop because the operator or the system said
   * so. */
  void request();

  /** @brief Asks the server to stop because a part of it failed, with what the
   * error said. */
  void fail(std::string text);

  /**
   * @brief Waits until the stop is asked for.
   * @returns Why, as the first request gave it. It returns at once when the
   * stop was asked for before the call.
   */
  [[nodiscard]] StopReason wait();

  /** @brief Says that the server finished stopping, which wakes the handlers
   * of the system that wait for it (see waitUntilStopped()). */
  void announceStopped();

  /**
   * @brief Waits until announceStopped() was called, at most @p timeout.
   * @returns Whether it was called.
   */
  [[nodiscard]] bool waitUntilStopped(engine::Duration timeout);

 private:
  void keepFirst(StopReason reason);

  std::mutex mutex_;
  std::condition_variable changed_;
  std::optional<StopReason> reason_;
  bool stopped_{false};
};

}  // namespace rtype::server
