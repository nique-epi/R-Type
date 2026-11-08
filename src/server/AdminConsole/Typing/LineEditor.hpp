#pragma once

#include <span>
#include <string>
#include "ConsoleChunk.hpp"
#include "EscapeState.hpp"
#include "LineEdit.hpp"

namespace rtype::server {

/**
 * @brief Keeps the line the administrator types in a terminal whose echo and
 * line editing are off (see ITerminal::enterTypingMode()), as the tty console
 * of ioquake3 keeps its field.
 *
 * Text is kept up to MAXIMUM_CONSOLE_LINE_LENGTH bytes; past it a byte is
 * refused and not shown, so a line is never dropped. Enter, '\r' or '\n', the
 * pair "\r\n" counting once, ends the line, even empty, as the line splitter
 * does. Backspace and Delete erase the last character, a whole UTF-8
 * character, shown as one column. An arrow or a function key (ESCAPE, '[' or
 * 'O', then bytes up to a final byte) and the other control characters, Tab
 * and Ctrl+D included, are dropped.
 */
class LineEditor {
 public:
  /** @returns The lines @p bytes ended, and the echo of every key. */
  [[nodiscard]] LineEdit feed(std::span<const char> bytes);

  /** @returns The text typed and not ended, as a last line, once the input
   * ended; no line when none was typed. */
  [[nodiscard]] ConsoleChunk finish();

  /** @returns The text typed and not ended yet. */
  [[nodiscard]] const std::string& typed() const;

 private:
  /** @returns Whether @p byte belongs to the sequence of a key, which it ends
   * or goes on. */
  bool skipSequence(char byte);
  /** @brief Applies @p byte, outside a sequence, to the line. */
  void read(char byte, LineEdit& edit);
  void endLine(LineEdit& edit);
  void erase(LineEdit& edit);
  void keep(char byte, LineEdit& edit);

  std::string typed_;
  EscapeState escape_{EscapeState::None};
  bool afterCarriageReturn_{false};
};

}  // namespace rtype::server
