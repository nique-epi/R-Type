#include <atomic>
#include <csignal>
#include "ServerException.hpp"
#include "StopSignals.hpp"

namespace rtype::server {

namespace {

/** @brief Whether a stop was asked for: a global, since a signal handler
 * reaches nothing else. */
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
std::atomic<bool> stopRequested{false};

static_assert(std::atomic<bool>::is_always_lock_free,
              "a signal handler may only store into a lock-free atomic");

/** @brief The handler of SIGINT and SIGTERM: records the request, nothing
 * else. */
void recordStopRequest([[maybe_unused]] int signalNumber) {
  stopRequested.store(true);
}

/** @brief Gives @p signalNumber back to its default handling, which ends the
 * program. */
void restoreDefaultHandling(int signalNumber) {
  static_cast<void>(std::signal(signalNumber, SIG_DFL));
}

}  // namespace

StopSignals::StopSignals() : requested_(&stopRequested) {
  stopRequested.store(false);
  if (std::signal(SIGINT, recordStopRequest) == SIG_ERR) {
    throw StopSignalsRefusedException();
  }
  if (std::signal(SIGTERM, recordStopRequest) == SIG_ERR) {
    restoreDefaultHandling(SIGINT);
    throw StopSignalsRefusedException();
  }
}

StopSignals::~StopSignals() {
  restoreDefaultHandling(SIGTERM);
  restoreDefaultHandling(SIGINT);
}

bool StopSignals::received() const { return requested_->load(); }

}  // namespace rtype::server
