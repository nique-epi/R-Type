#include "ProcessFixtures.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include <thread>
#include "IChildProcess.hpp"
#include "ProcessExit.hpp"
#include "ProcessTestConstants.hpp"
#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <cerrno>
#include <csignal>
#endif

std::optional<rtype::server::ProcessExit> waitForExit(
    rtype::server::IChildProcess& child, std::chrono::milliseconds timeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline) {
    if (const auto exit = child.exited()) {
      return exit;
    }
    std::this_thread::sleep_for(PROCESS_POLL_INTERVAL);
  }
  return child.exited();
}

#if defined(_WIN32)

bool processIsRunning(std::int64_t processId) {
  const HANDLE handle =
      ::OpenProcess(SYNCHRONIZE, FALSE, static_cast<DWORD>(processId));
  if (handle == nullptr) {
    return false;
  }
  const bool running = ::WaitForSingleObject(handle, 0) == WAIT_TIMEOUT;
  static_cast<void>(::CloseHandle(handle));
  return running;
}

#else

namespace {

/** @returns Whether /proc says the process is a zombie: it ended and waits for
 * its parent to collect it. Linux only; false where /proc does not exist. */
bool isZombie(std::int64_t processId) {
  std::ifstream stat{"/proc/" + std::to_string(processId) + "/stat"};
  std::string line;
  std::getline(stat, line);
  const std::size_t endOfName = line.rfind(')');
  return endOfName != std::string::npos && endOfName + 2 < line.size() &&
         line[endOfName + 2] == 'Z';
}

}  // namespace

bool processIsRunning(std::int64_t processId) {
  const bool exists =
      ::kill(static_cast<pid_t>(processId), 0) == 0 || errno == EPERM;
  return exists && !isZombie(processId);
}

#endif

std::optional<std::int64_t> waitForProcessIdIn(
    const std::string& path, std::chrono::milliseconds timeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline) {
    std::ifstream file{path};
    std::int64_t processId = 0;
    if (file >> processId) {
      return processId;
    }
    std::this_thread::sleep_for(PROCESS_POLL_INTERVAL);
  }
  return std::nullopt;
}

bool waitForProcessToEnd(std::int64_t processId,
                         std::chrono::milliseconds timeout) {
  const auto deadline = std::chrono::steady_clock::now() + timeout;
  while (std::chrono::steady_clock::now() < deadline) {
    if (!processIsRunning(processId)) {
      return true;
    }
    std::this_thread::sleep_for(PROCESS_POLL_INTERVAL);
  }
  return !processIsRunning(processId);
}
