#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace rtype::engine {

/**
 * @brief What reading the configuration did, for the program to log.
 */
struct ConfigurationReport {
  /** @brief Whether the file existed; a missing file is not an error. */
  bool fileFound{false};
  /** @brief How many lines ran, blank lines and comments not counted. */
  std::size_t executedLines{0};
  /** @brief One message per refused line, naming where the line came from. */
  std::vector<std::string> refusals;
  /** @brief The lines the rule of the source handed over to another program,
   * as they were written, in the order they were read: the lines of the file,
   * then the forced ones. */
  std::vector<std::string> handedOverLines;
};

}  // namespace rtype::engine
