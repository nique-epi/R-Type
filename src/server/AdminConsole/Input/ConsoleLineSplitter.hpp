#pragma once

#include <span>
#include <string>
#include "ConsoleChunk.hpp"

namespace rtype::server {

/**
 * @brief Cuts the bytes the console reads into lines. A line ends at LINE_END,
 * and a CARRIAGE_RETURN just before it is removed, so a line typed on Windows
 * reads as on Linux. A line longer than MAXIMUM_CONSOLE_LINE_LENGTH bytes, its
 * CARRIAGE_RETURN counted, is dropped whole, never cut: a piece of a command
 * is another command. What it keeps between two calls is never longer than
 * that.
 */
class ConsoleLineSplitter {
 public:
  /** @returns The lines @p bytes completes, the text kept from the previous
   * calls in front of the first one. */
  [[nodiscard]] ConsoleChunk feed(std::span<const char> bytes);

  /** @returns The text after the last end of line, as a last line, when the
   * input ended without one; no line when nothing is left. */
  [[nodiscard]] ConsoleChunk finish();

 private:
  /** @brief Adds @p character to the line being read; drops that line once
   * it is too long. */
  void keep(char character);
  /** @brief Ends the line being read: into @p chunk, or counted as
   * dropped. */
  void endLine(ConsoleChunk& chunk);

  std::string pending_;
  bool overlong_{false};
};

}  // namespace rtype::server
