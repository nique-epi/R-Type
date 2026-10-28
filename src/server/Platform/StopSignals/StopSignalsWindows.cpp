#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <atomic>
#include <memory>
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"

namespace rtype::server {

namespace {

/** The request the handler of the system reaches; the system gives the handler
 * no argument of ours. */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<StopRequest*> activeRequest{nullptr};

/**
 * Runs on a thread the system creates for each event. Ctrl-C and Ctrl-Break
 * have no time limit: the request is enough. The closing of the console ends
 * the process when the handler returns, or after a few seconds, so the handler
 * waits for the program to finish stopping, at most CONSOLE_CLOSE_WAIT.
 * Logoff and shutdown only reach services, and are handled the same way.
 */
BOOL WINAPI handleConsoleEvent(DWORD eventType) {
  StopRequest* const request = activeRequest.load();
  if (request == nullptr) {
    return FALSE;
  }
  switch (eventType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      request->request();
      return TRUE;
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
      request->request();
      static_cast<void>(request->waitUntilStopped(CONSOLE_CLOSE_WAIT));
      return TRUE;
    default:
      return FALSE;
  }
}

}  // namespace

/** @brief Registers handleConsoleEvent() for the life of the object. */
class StopSignalsHandle {
 public:
  explicit StopSignalsHandle(StopRequest& request) {
    activeRequest = &request;
    if (SetConsoleCtrlHandler(handleConsoleEvent, TRUE) == 0) {
      activeRequest = nullptr;
      throw ConsoleHandlerException(GetLastError());
    }
  }

  /** Takes the handler out, then forgets the request: a handler running now
   * has already read it and returns once the program announced the stop. */
  ~StopSignalsHandle() {
    static_cast<void>(SetConsoleCtrlHandler(handleConsoleEvent, FALSE));
    activeRequest = nullptr;
  }

  StopSignalsHandle(const StopSignalsHandle&) = delete;
  StopSignalsHandle& operator=(const StopSignalsHandle&) = delete;
  StopSignalsHandle(StopSignalsHandle&&) = delete;
  StopSignalsHandle& operator=(StopSignalsHandle&&) = delete;
};

StopSignals::StopSignals(StopRequest& request)
    : handle_(std::make_unique<StopSignalsHandle>(request)) {}

StopSignals::~StopSignals() = default;

}  // namespace rtype::server
