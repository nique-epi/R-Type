#pragma once

#include <span>
#include "ConsoleChunk.hpp"

namespace rtype::server {

/** @brief What turns the bytes the console reads into lines: the line
 * splitter, or the screen of a terminal in typing mode (see TerminalScreen). */
class ILineCutter {
 public:
  virtual ~ILineCutter() = default;

  /** @returns The lines @p bytes completes. */
  [[nodiscard]] virtual ConsoleChunk feed(std::span<const char> bytes) = 0;

  /** @returns What is left as a last line, once the input ended. */
  [[nodiscard]] virtual ConsoleChunk finish() = 0;
};

}  // namespace rtype::server
