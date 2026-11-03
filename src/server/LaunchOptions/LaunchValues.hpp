#pragma once

#include <string>
#include <vector>

namespace rtype::server {

/**
 * @brief What the launch options of the server set while the arguments are
 * read: each option writes its own part.
 */
struct LaunchValues {
  /** @brief The lines of the command `set` the options force, in order. */
  std::vector<std::string> forcedLines;
  /** @brief Whether `--help` was given. */
  bool helpRequested{false};
};

}  // namespace rtype::server
