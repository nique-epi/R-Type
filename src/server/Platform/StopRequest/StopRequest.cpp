#include "StopRequest.hpp"
#include <mutex>
#include <string>
#include <utility>
#include "StopReason.hpp"
#include "TimeConstants.hpp"

namespace rtype::server {

void StopRequest::request() { keepFirst(StopReason{}); }

void StopRequest::fail(std::string text) {
  keepFirst(StopReason{.failed = true, .text = std::move(text)});
}

StopReason StopRequest::wait() {
  std::unique_lock lock{mutex_};
  changed_.wait(lock, [this] { return reason_.has_value(); });
  return *reason_;
}

void StopRequest::announceStopped() {
  {
    const std::scoped_lock lock{mutex_};
    stopped_ = true;
  }
  changed_.notify_all();
}

bool StopRequest::waitUntilStopped(engine::Duration timeout) {
  std::unique_lock lock{mutex_};
  return changed_.wait_for(lock, timeout, [this] { return stopped_; });
}

void StopRequest::keepFirst(StopReason reason) {
  {
    const std::scoped_lock lock{mutex_};
    if (reason_.has_value()) {
      return;
    }
    reason_ = std::move(reason);
  }
  changed_.notify_all();
}

}  // namespace rtype::server
