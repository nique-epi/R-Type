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
};

}  // namespace rtype::engine
