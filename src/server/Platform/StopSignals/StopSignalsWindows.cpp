#include <windows.h>
#include <atomic>
#include "ServerException.hpp"
#include "StopSignals.hpp"

namespace rtype::server {

namespace {

/** @brief Whether a stop was asked for: a global, since a console control
 * handler reaches nothing else. */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<bool> stopRequested{false};

static_assert(std::atomic<bool>::is_always_lock_free,
              "a signal handler may only store into a lock-free atomic");

/**
 * @brief The console control handler: records the request on Ctrl-C,
 * Ctrl-Break and the closing of the console, and leaves the other events to
 * the next handler. After the closing of the console it waits, so Windows
 * does not end the process before the program stopped by itself.
 */
BOOL WINAPI recordStopRequest(DWORD controlType) {
  switch (controlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      stopRequested.store(true);
      return TRUE;
    case CTRL_CLOSE_EVENT:
      stopRequested.store(true);
      Sleep(INFINITE);
      return TRUE;
    default:
      return FALSE;
  }
}

}  // namespace

StopSignals::StopSignals() : requested_(&stopRequested) {
  stopRequested.store(false);
  if (SetConsoleCtrlHandler(recordStopRequest, TRUE) == 0) {
    throw StopSignalsRefusedException();
  }
}

StopSignals::~StopSignals() {
  static_cast<void>(SetConsoleCtrlHandler(recordStopRequest, FALSE));
}

bool StopSignals::received() const { return requested_->load(); }

}  // namespace rtype::server
