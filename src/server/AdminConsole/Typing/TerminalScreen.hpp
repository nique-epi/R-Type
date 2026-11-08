#pragma once

#include <mutex>
#include <span>
#include <string_view>
#include "ConsoleChunk.hpp"
#include "ILineCutter.hpp"
#include "ITerminal.hpp"
#include "LineEditor.hpp"

namespace rtype::server {

/**
 * @brief The terminal of the admin console in typing mode: it shows the prompt
 * and the line being typed (see LineEditor), and writes every other line above
 * them, as CON_Hide and CON_Show of ioquake3 and printAbove() of JLine do:
 * erase the row, write the line, write the prompt and the typed text again.
 *
 * Each call takes one lock and writes once to the terminal, so the thread that
 * reads the keyboard, the simulation thread and the threads that write to the
 * journal or relay a game may call it at once. It never writes to the journal:
 * the journal calls writeAbove() under its own lock.
 *
 * A typed line wider than the terminal wraps, and only its last row is erased
 * before a line is written above it.
 */
class TerminalScreen final : public ILineCutter {
 public:
  /** @param terminal Where it writes; it must outlive the screen. */
  explicit TerminalScreen(ITerminal& terminal);

  /** @brief Writes CONSOLE_READY_TEXT, then the prompt. */
  void open();

  /** @brief Edits the typed line with @p bytes and shows the edit.
   * @returns The lines ended. */
  [[nodiscard]] ConsoleChunk feed(std::span<const char> bytes) override;

  [[nodiscard]] ConsoleChunk finish() override;

  /** @brief Writes @p line, without its end, above the prompt and the typed
   * line. */
  void writeAbove(std::string_view line);

  /** @brief Erases the prompt and the typed line, as the console stops. */
  void close();

 private:
  std::mutex mutex_;
  ITerminal* terminal_;
  LineEditor editor_;
};

}  // namespace rtype::server
