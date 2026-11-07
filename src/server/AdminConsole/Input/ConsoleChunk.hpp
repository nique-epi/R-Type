#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace rtype::server {

/** @brief What the bytes read once completed: the lines to run, and how many
 * lines were too long to keep. */
struct ConsoleChunk {
  /** @brief The lines completed, oldest first, without their end of line. */
  std::vector<std::string> lines;
  /** @brief The lines longer than MAXIMUM_CONSOLE_LINE_LENGTH, dropped
   * whole. */
  std::size_t droppedLines{0};
};

}  // namespace rtype::server
