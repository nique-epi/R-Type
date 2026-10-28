// misc-include-cleaner wants <signal.h> for the POSIX signal API, which
// modernize-deprecated-headers rejects in C++: the checks cannot both be met.
// NOLINTBEGIN(misc-include-cleaner)
#include <pthread.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <csignal>
#include <memory>
#include <system_error>
#include <thread>
#include "ServerException.hpp"
#include "StopRequest.hpp"
#include "StopSignals.hpp"

namespace rtype::server {

namespace {

/** Takes each of the signals that is pending: sigwait() returns at once for a
 * pending signal, and macOS has no sigtimedwait(). */
void takePendingSignals() {
  sigset_t pending{};
  sigpending(&pending);
  for (const int signalNumber : {SIGINT, SIGTERM}) {
    if (sigismember(&pending, signalNumber) == 1) {
      sigset_t one{};
      sigemptyset(&one);
      sigaddset(&one, signalNumber);
      int taken = 0;
      sigwait(&one, &taken);
    }
  }
}

}  // namespace

/**
 * @brief Blocks SIGINT and SIGTERM in the calling thread, and keeps the thread
 * that takes them with sigwait() and turns the first into a stop request.
 */
class StopSignalsHandle {
 public:
  explicit StopSignalsHandle(StopRequest& request) {
    sigemptyset(&signals_);
    sigaddset(&signals_, SIGINT);
    sigaddset(&signals_, SIGTERM);
    const int error = pthread_sigmask(SIG_BLOCK, &signals_, &previousMask_);
    if (error != 0) {
      throw SignalMaskException(std::generic_category().message(error));
    }
    waiter_ = std::jthread{[this, &request] { waitForASignal(request); }};
  }

  /**
   * Wakes the thread if no signal reached it, with a SIGTERM that only it can
   * take, joins it, takes the signals still pending so the mask restored below
   * does not deliver them with their default action, and gives the mask back.
   */
  ~StopSignalsHandle() {
    if (!received_) {
      kill(getpid(), SIGTERM);
    }
    waiter_.join();
    takePendingSignals();
    static_cast<void>(pthread_sigmask(SIG_SETMASK, &previousMask_, nullptr));
  }

  StopSignalsHandle(const StopSignalsHandle&) = delete;
  StopSignalsHandle& operator=(const StopSignalsHandle&) = delete;
  StopSignalsHandle(StopSignalsHandle&&) = delete;
  StopSignalsHandle& operator=(StopSignalsHandle&&) = delete;

 private:
  void waitForASignal(StopRequest& request) {
    int received = 0;
    if (sigwait(&signals_, &received) != 0) {
      request.fail("the system refused to wait for the stop signals");
      received_ = true;
      return;
    }
    received_ = true;
    request.request();
  }

  sigset_t signals_{};
  sigset_t previousMask_{};
  std::atomic<bool> received_{false};
  std::jthread waiter_;
};

StopSignals::StopSignals(StopRequest& request)
    : handle_(std::make_unique<StopSignalsHandle>(request)) {}

StopSignals::~StopSignals() = default;

}  // namespace rtype::server
// NOLINTEND(misc-include-cleaner)
