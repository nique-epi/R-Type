#include <windows.h>
#include <atomic>
#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>
#include <thread>
#include "AdminConsoleConstants.hpp"
#include "StandardInput.hpp"

namespace rtype::server {

namespace {

/**
 * @brief Cancels the read of @p reader until @p leftRead says it is over: a
 * cancel that comes before ReadFile() starts finds nothing (ERROR_NOT_FOUND),
 * so it is tried again.
 */
void cancelUntilLeft(HANDLE reader, const std::atomic<bool>& leftRead) {
  while (!leftRead.load()) {
    static_cast<void>(::CancelSynchronousIo(reader));
    std::this_thread::sleep_for(CONSOLE_CANCEL_RETRY);
  }
}

/** @returns A handle on the calling thread that CancelSynchronousIo()
 * accepts (THREAD_TERMINATE), or nullptr. */
HANDLE callingThread() {
  HANDLE thread = nullptr;
  if (::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(),
                        ::GetCurrentProcess(), &thread, THREAD_TERMINATE, FALSE,
                        0) == FALSE) {
    return nullptr;
  }
  return thread;
}

/**
 * @returns The bytes one ReadFile() put in @p buffer: 0 when Ctrl+C or the
 * stop ended it with nothing read; nothing when the input ended or failed.
 */
std::optional<std::size_t> readOnce(HANDLE input, std::span<char> buffer,
                                    const std::stop_token& stop) {
  const HANDLE reader = callingThread();
  if (reader == nullptr) {
    return std::nullopt;
  }
  std::atomic<bool> leftRead{false};
  const std::thread::id readerId = std::this_thread::get_id();
  DWORD count = 0;
  BOOL done = FALSE;
  DWORD error = ERROR_SUCCESS;
  {
    const std::stop_callback cancel{
        stop, [reader, readerId, &leftRead] {
          if (std::this_thread::get_id() != readerId) {
            cancelUntilLeft(reader, leftRead);
          }
        }};
    if (!stop.stop_requested()) {
      ::SetLastError(ERROR_SUCCESS);
      done = ::ReadFile(input, buffer.data(), static_cast<DWORD>(buffer.size()),
                        &count, nullptr);
      error = ::GetLastError();
    }
    leftRead.store(true);
  }
  static_cast<void>(::CloseHandle(reader));
  if (count == 0 && error != ERROR_OPERATION_ABORTED) {
    return std::nullopt;
  }
  return static_cast<std::size_t>(done == FALSE ? 0 : count);
}

}  // namespace

std::optional<std::size_t> StandardInput::read(std::span<char> buffer,
                                               const std::stop_token& stop) {
  const HANDLE input = ::GetStdHandle(STD_INPUT_HANDLE);
  if (input == nullptr || input == INVALID_HANDLE_VALUE) {
    return std::nullopt;
  }
  std::optional<std::size_t> received{0};
  while (received == std::size_t{0} && !stop.stop_requested()) {
    received = readOnce(input, buffer, stop);
  }
  if (stop.stop_requested()) {
    return std::nullopt;
  }
  return received;
}

}  // namespace rtype::server
