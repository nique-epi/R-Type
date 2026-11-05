#pragma once

#include <cstdint>

// Compile-time minimum log level. CMake passes `LOG_LEVEL=name` and
// translates it to a 0..5 integer here. Methods of lower severity expand
// to nothing thanks to `if constexpr` in Logger.hpp.
//   0=Trace 1=Debug 2=Info 3=Warn 4=Error 5=Silent
#ifndef RT_LOG_BUILD_LEVEL
#define RT_LOG_BUILD_LEVEL 0
#endif

namespace rtype::logging {

/** @brief How serious a log line is; a line below the level of the program
 * is not written. */
enum class LogLevel : std::uint8_t {
  Trace = 0,
  Debug = 1,
  Info = 2,
  Warn = 3,
  Error = 4,
  Silent = 5,
};

constexpr LogLevel buildMinLevel = static_cast<LogLevel>(RT_LOG_BUILD_LEVEL);

constexpr bool isCompiledIn(LogLevel candidate) {
  return static_cast<std::uint8_t>(candidate) >=
         static_cast<std::uint8_t>(buildMinLevel);
}

}  // namespace rtype::logging
