#include <windows.h>
#include <memory>
#include <optional>
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
  return SavedTerminalModes{.input = *input,
                            .output = *output,
                            .inputCodePage = ::GetConsoleCP(),
                            .outputCodePage = ::GetConsoleOutputCP()};
}

/** @returns Whether the console now writes VT sequences, reads each key as it
 * is pressed without echo, and reads and writes UTF-8, so the line editor
 * erases a whole character whatever the code page of the console was; part of
 * it may be done when false. */
bool switchToTyping(const SavedTerminalModes& modes) {
  return ::SetConsoleMode(::GetStdHandle(STD_OUTPUT_HANDLE),
                          modes.output | ENABLE_VIRTUAL_TERMINAL_PROCESSING) !=
             FALSE &&
         ::SetConsoleMode(
             ::GetStdHandle(STD_INPUT_HANDLE),
             modes.input & ~static_cast<DWORD>(ENABLE_LINE_INPUT |
                                               ENABLE_ECHO_INPUT)) != FALSE &&
         ::SetConsoleCP(CP_UTF8) != FALSE &&
         ::SetConsoleOutputCP(CP_UTF8) != FALSE;
}

}  // namespace

bool StandardTerminal::enterTypingMode() {
  const std::optional<SavedTerminalModes> modes = consoleModes();
  if (!modes.has_value()) {
    return false;
  }
  saved_ = std::make_unique<SavedTerminalModes>(*modes);
  if (!switchToTyping(*modes)) {
    leaveTypingMode();
    return false;
  }
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
  static_cast<void>(::SetConsoleCP(saved_->inputCodePage));
  static_cast<void>(::SetConsoleOutputCP(saved_->outputCodePage));
  saved_.reset();
}

}  // namespace rtype::server
