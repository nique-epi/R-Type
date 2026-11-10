#include "RunFixtures.hpp"
#include <chrono>
#include <functional>
#include <thread>
#include "RunTestConstants.hpp"

namespace rtype::client::testing {

bool heldWithinTheWait(const std::function<bool()>& condition) {
  const auto end = std::chrono::steady_clock::now() + THREAD_WAIT;
  while (!condition()) {
    if (std::chrono::steady_clock::now() >= end) {
      return false;
    }
    std::this_thread::sleep_for(THREAD_POLL);
  }
  return true;
}

}  // namespace rtype::client::testing
