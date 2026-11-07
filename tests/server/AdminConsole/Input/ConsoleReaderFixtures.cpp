#include "ConsoleReaderFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <string>
#include <thread>
#include <vector>
#include "AdminConsoleTestConstants.hpp"
#include "BoundedQueue.hpp"
#include "ServerModuleTestConstants.hpp"

std::vector<std::string> linesWithin(
    rtype::concurrency::BoundedQueue<std::string>& lines, std::size_t count) {
  std::vector<std::string> received;
  const auto deadline = std::chrono::steady_clock::now() + WAIT_FOR_THREADS;
  lines.drainInto(received);
  while (received.size() < count &&
         std::chrono::steady_clock::now() < deadline) {
    std::this_thread::sleep_for(CONSOLE_LOOK_INTERVAL);
    lines.drainInto(received);
  }
  return received;
}
