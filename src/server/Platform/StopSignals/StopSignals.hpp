#pragma once

#include <memory>
#include "StopRequest.hpp"

namespace rtype::server {

class StopSignalsHandle;

/**
 * @brief While it lives, turns the signals that ask a program to stop into a
 * request on a StopRequest: Ctrl-C and SIGTERM on Linux and macOS, Ctrl-C,
 * Ctrl-Break and the closing of the console on Windows.
 *
 * Build it before any thread of the program exists. On Linux and macOS it
 * blocks SIGINT and SIGTERM in the calling thread, which every thread created
 * afterwards inherits, and a thread of its own takes them with sigwait(), so
 * nothing runs inside a signal handler. On Windows the system runs the handler
 * on a new thread; for the closing of the console, which the system ends after
 * CONSOLE_CLOSE_WAIT at most, the handler returns only once the program called
 * StopRequest::announceStopped() (see HandlerRoutine on learn.microsoft.com).
 * The request must outlive the object. One object at a time.
 */
class StopSignals {
 public:
  /** @throws SignalMaskException when the system refuses to block the signals
   * (Linux and macOS), or ConsoleHandlerException when it refuses the handler
   * (Windows). */
  explicit StopSignals(StopRequest& request);
  ~StopSignals();

  StopSignals(const StopSignals&) = delete;
  StopSignals& operator=(const StopSignals&) = delete;
  StopSignals(StopSignals&&) = delete;
  StopSignals& operator=(StopSignals&&) = delete;

 private:
  std::unique_ptr<StopSignalsHandle> handle_;
};

}  // namespace rtype::server
