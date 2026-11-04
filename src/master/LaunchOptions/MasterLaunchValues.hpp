#pragma once

#include <string>
#include <vector>

namespace rtype::master {

/** @brief What the launch options of the master record, as they are read. */
struct MasterLaunchValues {
  /** @brief The lines of the command `set` the options force, in order. */
  std::vector<std::string> forcedLines;
  /** @brief Whether `--help` was given. */
  bool helpRequested{false};
};

}  // namespace rtype::master
