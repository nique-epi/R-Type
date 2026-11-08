#pragma once

#include <memory>
#include <string_view>
#include "ITerminal.hpp"

namespace rtype::server {

struct SavedTerminalModes;

/**
 * @brief The terminal of the supervisor: its standard input, output and error.
 *
 * On Linux and macOS, enterTypingMode() turns ICANON and ECHO off in the modes
 * of the input, with VMIN 1 and VTIME 0, as the tty console of ioquake3 does;
 * ISIG stays, so Ctrl+C still stops the supervisor, and OPOST stays, so "\n"
 * still starts a row. It does so only in the foreground of its terminal: a
 * process of the background that changes the modes is stopped by SIGTTOU.
 * SIGTTOU is ignored while the modes are given back, in case the shell sent
 * the supervisor to the background since.
 *
 * On Windows, it turns ENABLE_LINE_INPUT and ENABLE_ECHO_INPUT off on the
 * input, keeping ENABLE_PROCESSED_INPUT for Ctrl+C, and
 * ENABLE_VIRTUAL_TERMINAL_PROCESSING on for the output, which the erase of a
 * row needs, and makes the console read and write UTF-8 (code page 65001)
 * until the modes are given back. That file is checked by the Windows job of
 * the CI only.
 *
 * It enters typing mode only when the input, the output and the error are
 * one terminal (the same device on Linux and macOS).
 *
 * The destructor gives the modes back too. A supervisor that crashes leaves
 * its terminal without echo: `reset` brings it back.
 */
class StandardTerminal final : public ITerminal {
 public:
  StandardTerminal();
  ~StandardTerminal() override;

  StandardTerminal(const StandardTerminal&) = delete;
  StandardTerminal& operator=(const StandardTerminal&) = delete;
  StandardTerminal(StandardTerminal&&) = delete;
  StandardTerminal& operator=(StandardTerminal&&) = delete;

  [[nodiscard]] bool enterTypingMode() override;
  void leaveTypingMode() override;
  void write(std::string_view text) override;

 private:
  std::unique_ptr<SavedTerminalModes> saved_;
};

}  // namespace rtype::server
