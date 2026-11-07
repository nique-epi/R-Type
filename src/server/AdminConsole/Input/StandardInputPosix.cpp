// misc-include-cleaner wants the internal headers of the C library for the
// POSIX select(), pipe and signal API (sys/_types/_fd_set.h on macOS, and
// <signal.h>, which modernize-deprecated-headers rejects in C++), as in
// StopSignalsPosix.cpp: the checks cannot both be met.
// NOLINTBEGIN(misc-include-cleaner)
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>
#include "AdminConsoleConstants.hpp"
#include "StandardInput.hpp"

namespace rtype::server {

namespace {

/**
 * @returns What select() found ready among @p wake and, when @p withInput, the
 * standard input, waiting at most @p timeout, forever when it is null; nothing
 * when select() failed for good. A signal that interrupts select() starts it
 * again.
 */
std::optional<fd_set> readyAmong(int wake, bool withInput, timeval* timeout) {
  while (true) {
    fd_set watched;
    FD_ZERO(&watched);
    if (withInput) {
      FD_SET(STDIN_FILENO, &watched);
    }
    FD_SET(wake, &watched);
    if (::select(wake + 1, &watched, nullptr, nullptr, timeout) >= 0) {
      return watched;
    }
    if (errno != EINTR) {
      return std::nullopt;
    }
  }
}

/** @returns Whether the standard input has bytes to read or reached its end;
 * false when the byte of @p wake came first, or select() failed for good. */
bool inputReady(int wake) {
  std::optional<fd_set> ready = readyAmong(wake, true, nullptr);
  return ready.has_value() && FD_ISSET(wake, &ready.value()) == 0;
}

/** @brief Waits CONSOLE_BACKGROUND_WAIT, or less when the byte of @p wake
 * comes, without reading the standard input. */
void pauseUnlessWoken(int wake) {
  timeval pause{.tv_sec = 0,
                .tv_usec = static_cast<suseconds_t>(
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        CONSOLE_BACKGROUND_WAIT)
                        .count())};
  static_cast<void>(readyAmong(wake, false, &pause));
}

/** @returns Whether the standard input is an open descriptor: a supervisor
 * started with it closed has no console. isatty() fails with EBADF only on a
 * closed one. */
bool inputIsOpen() { return ::isatty(STDIN_FILENO) == 1 || errno != EBADF; }

/**
 * @returns Whether @p wake now holds the two ends of a pipe that a game
 * process started meanwhile does not inherit: pipe2() with O_CLOEXEC on Linux;
 * on macOS, which has no pipe2(), pipe() then FD_CLOEXEC on each end, so a
 * game started between the two calls inherits the pipe, which it never reads.
 */
bool openWakePipe(std::array<int, 2>& wake) {
#ifdef __linux__
  return ::pipe2(wake.data(), O_CLOEXEC) == 0;
#else
  if (::pipe(wake.data()) != 0) {  // NOLINT(android-cloexec-pipe)
    return false;
  }
  for (const int end : wake) {
    static_cast<void>(
        ::fcntl(  // NOLINT(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
            end, F_SETFD, FD_CLOEXEC));
  }
  return true;
#endif
}

/** @returns Whether a read that failed with @p error may be tried again: an
 * interruption, or the refusal a process in the background gets. */
bool isPassing(int error) {
  return error == EINTR || error == EAGAIN || error == EIO;
}

/**
 * @returns The bytes read into @p buffer; nothing once the input ended or
 * failed for good, or when the byte of @p wake came. A read refused because
 * the supervisor runs in the background waits CONSOLE_BACKGROUND_WAIT first.
 */
std::optional<std::size_t> readUntilWoken(std::span<char> buffer, int wake) {
  while (inputReady(wake)) {
    const ssize_t count = ::read(STDIN_FILENO, buffer.data(), buffer.size());
    if (count > 0) {
      return static_cast<std::size_t>(count);
    }
    const int error = errno;
    if (count == 0 || !isPassing(error)) {
      return std::nullopt;
    }
    if (error == EIO) {
      pauseUnlessWoken(wake);
    }
  }
  return std::nullopt;
}

}  // namespace

std::optional<std::size_t> StandardInput::read(std::span<char> buffer,
                                               const std::stop_token& stop) {
  std::array<int, 2> wake{};
  if (!inputIsOpen() || !openWakePipe(wake)) {
    return std::nullopt;
  }
  const auto previous = std::signal(SIGTTIN, SIG_IGN);
  std::optional<std::size_t> received;
  {
    const std::stop_callback wakeOnStop{
        stop, [&wake] { static_cast<void>(::write(wake[1], &WAKE_BYTE, 1)); }};
    received = readUntilWoken(buffer, wake[0]);
  }
  if (previous != SIG_ERR) {
    static_cast<void>(std::signal(SIGTTIN, previous));
  }
  static_cast<void>(::close(wake[0]));
  static_cast<void>(::close(wake[1]));
  return received;
}

}  // namespace rtype::server
// NOLINTEND(misc-include-cleaner)
