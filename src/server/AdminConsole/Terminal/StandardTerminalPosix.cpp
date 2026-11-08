// misc-include-cleaner wants the internal headers of the C library for the
// POSIX terminal and signal API, as in StandardInputPosix.cpp.
// NOLINTBEGIN(misc-include-cleaner)
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <memory>
#include <optional>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "SavedTerminalModes.hpp"
#include "StandardTerminal.hpp"

namespace rtype::server {

namespace {

/** @returns Whether the input, the output and the error are one terminal: three
 * terminals of the same device. */
bool onOneTerminal() {
  struct stat input{};
  struct stat output{};
  struct stat error{};
  return ::isatty(STDIN_FILENO) == 1 && ::isatty(STDOUT_FILENO) == 1 &&
         ::isatty(STDERR_FILENO) == 1 && ::fstat(STDIN_FILENO, &input) == 0 &&
         ::fstat(STDOUT_FILENO, &output) == 0 &&
         ::fstat(STDERR_FILENO, &error) == 0 &&
         input.st_rdev == output.st_rdev && input.st_rdev == error.st_rdev;
}

/** @returns The modes of the terminal of the input, when the input, the
 * output and the error are one terminal and this process holds its
 * foreground; nothing otherwise. */
std::optional<termios> foregroundModes() {
  termios modes{};
  if (!onOneTerminal() || ::tcgetpgrp(STDIN_FILENO) != ::getpgrp() ||
      ::tcgetattr(STDIN_FILENO, &modes) != 0) {
    return std::nullopt;
  }
  return modes;
}

/** @returns @p modes without the line editing and the echo of the terminal:
 * each key is read as it is pressed. */
termios typingModesOf(termios modes) {
  modes.c_lflag &=
      ~(static_cast<tcflag_t>(ICANON) | static_cast<tcflag_t>(ECHO));
  modes.c_cc[VMIN] = TYPING_MINIMUM_BYTES;
  modes.c_cc[VTIME] = TYPING_TIMEOUT;
  return modes;
}

}  // namespace

bool StandardTerminal::enterTypingMode() {
  const std::optional<termios> modes = foregroundModes();
  if (!modes.has_value()) {
    return false;
  }
  auto saved =
      std::make_unique<SavedTerminalModes>(SavedTerminalModes{.input = *modes});
  const termios typing = typingModesOf(*modes);
  if (::tcsetattr(STDIN_FILENO, TCSANOW, &typing) != 0) {
    return false;
  }
  saved_ = std::move(saved);
  return true;
}

void StandardTerminal::leaveTypingMode() {
  if (saved_ == nullptr) {
    return;
  }
  const auto previous = std::signal(SIGTTOU, SIG_IGN);
  static_cast<void>(::tcsetattr(STDIN_FILENO, TCSANOW, &saved_->input));
  if (previous != SIG_ERR) {
    static_cast<void>(std::signal(SIGTTOU, previous));
  }
  saved_.reset();
}

}  // namespace rtype::server
// NOLINTEND(misc-include-cleaner)
