#pragma once

#include <string_view>

namespace rtype::server {

/** @brief The terminal the admin console reads and writes: the standard input
 * and output of the supervisor, or what a test records. */
class ITerminal {
 public:
  virtual ~ITerminal() = default;

  /**
   * @brief Turns the echo and the line editing of the terminal off, so the
   * console edits the line itself (see LineEditor); Ctrl+C still stops the
   * program.
   * @returns Whether it did: false when the input, the output or the error is
   * no terminal, or the program runs in the background of a shell.
   */
  [[nodiscard]] virtual bool enterTypingMode() = 0;

  /** @brief Gives the terminal back the modes it had before
   * enterTypingMode(); nothing when it never entered. */
  virtual void leaveTypingMode() = 0;

  /** @brief Writes @p text as it is, escape sequences included, and flushes
   * it. */
  virtual void write(std::string_view text) = 0;
};

}  // namespace rtype::server
