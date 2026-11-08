#include <windows.h>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include "SavedTerminalModes.hpp"
#include "StandardTerminal.hpp"

namespace rtype::server {

namespace {

/** @returns The modes of the console @p handle; nothing when it is no
 * console. */
std::optional<DWORD> consoleModeOf(HANDLE handle) {
  DWORD mode = 0;
  if (handle == nullptr || handle == INVALID_HANDLE_VALUE ||
      ::GetConsoleMode(handle, &mode) == FALSE) {
    return std::nullopt;
  }
  return mode;
}

/** @returns The modes of the input and the output, when the input, the output
 * and the error are consoles; nothing otherwise. */
std::optional<SavedTerminalModes> consoleModes() {
  const std::optional<DWORD> input =
      consoleModeOf(::GetStdHandle(STD_INPUT_HANDLE));
  const std::optional<DWORD> output =
      consoleModeOf(::GetStdHandle(STD_OUTPUT_HANDLE));
  if (!input.has_value() || !output.has_value() ||
      !consoleModeOf(::GetStdHandle(STD_ERROR_HANDLE)).has_value()) {
    return std::nullopt;
  }
  return SavedTerminalModes{.input = *input, .output = *output};
}

}  // namespace

StandardTerminal::StandardTerminal() = default;

StandardTerminal::~StandardTerminal() { leaveTypingMode(); }

bool StandardTerminal::enterTypingMode() {
  const std::optional<SavedTerminalModes> modes = consoleModes();
  if (!modes.has_value()) {
    return false;
  }
  HANDLE output = ::GetStdHandle(STD_OUTPUT_HANDLE);
  if (::SetConsoleMode(output,
                       modes->output | ENABLE_VIRTUAL_TERMINAL_PROCESSING) ==
      FALSE) {
    return false;
  }
  if (::SetConsoleMode(::GetStdHandle(STD_INPUT_HANDLE),
                       modes->input & ~static_cast<DWORD>(ENABLE_LINE_INPUT |
                                                          ENABLE_ECHO_INPUT)) ==
      FALSE) {
    static_cast<void>(::SetConsoleMode(output, modes->output));
    return false;
  }
  saved_ = std::make_unique<SavedTerminalModes>(*modes);
  return true;
}

void StandardTerminal::leaveTypingMode() {
  if (saved_ == nullptr) {
    return;
  }
  static_cast<void>(
      ::SetConsoleMode(::GetStdHandle(STD_INPUT_HANDLE), saved_->input));
  static_cast<void>(
      ::SetConsoleMode(::GetStdHandle(STD_OUTPUT_HANDLE), saved_->output));
  saved_.reset();
}

void StandardTerminal::write(std::string_view text) {
  std::cout << text << std::flush;
}

}  // namespace rtype::server
