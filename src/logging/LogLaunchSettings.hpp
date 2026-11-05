#pragma once

#include <optional>
#include "LogLevel.hpp"
#include "Logger.hpp"

namespace rtype::logging {

/**
 * @brief What the logging options of a launch asked for, before they are
 * applied to the Logger.
 */
struct LogLaunchSettings {
  std::optional<LogLevel> level;
  LogOutput output;
  /** @brief True unless `--log-file` named the journal: only the default
   * journal may be left out when it cannot be opened. */
  bool journalIsDefault{true};
};

}  // namespace rtype::logging
