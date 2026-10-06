/**
 * @file Logger.hpp
 * @brief Pino/Nest-style structured logger, instance-based.
 *
 * Each module of the codebase owns a `Logger` instance whose module
 * name is supplied at construction:
 *
 * ```cpp
 * class NetworkContext {
 *  private:
 *     rtype::logging::Logger logger_{"Network"};
 * };
 *
 * logger_.info("listening on port ", port);
 * logger_.warn("dropped a packet of ", size, " bytes");
 * auto timer = logger_.scope("tick");
 * ```
 *
 * Output line format: `[HH:MM:SS.mmm] [LEVEL] [Module] - body`.
 *
 * Two filter layers:
 *
 * - **Compile-time** (`RT_LOG_BUILD_LEVEL`, set by CMake from `LOG_LEVEL`):
 *   each logging method is wrapped in `if constexpr` so calls below the
 *   build level produce no code at all. Default `0` (Trace, everything
 *   compiled in).
 *
 * - **Runtime** (`Logger::setLevel()` / env `RT_LOG_LEVEL`): classic
 *   filter checked at every call that survived compile-time. Default
 *   `Info` so debug output is silent unless explicitly enabled.
 */

#pragma once

#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

// Compile-time minimum log level. CMake passes `LOG_LEVEL=name` and
// translates it to a 0..5 integer here. Methods of lower severity expand
// to nothing thanks to `if constexpr` below.
//   0=Trace 1=Debug 2=Info 3=Warn 4=Error 5=Silent
#ifndef RT_LOG_BUILD_LEVEL
#define RT_LOG_BUILD_LEVEL 0
#endif

namespace rtype::logging {

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

class Logger {
 public:
  /**
   * @brief Build a logger bound to @p module. The module name is paid
   *        once at construction and reused for every subsequent call.
   */
  explicit Logger(std::string module);

  static LogLevel level();
  static void setLevel(LogLevel newLevel);
  static bool shouldLog(LogLevel candidate);

  template <class... Args>
  void trace(Args&&... args) const {
    if constexpr (isCompiledIn(LogLevel::Trace)) {
      emit(LogLevel::Trace, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  void debug(Args&&... args) const {
    if constexpr (isCompiledIn(LogLevel::Debug)) {
      emit(LogLevel::Debug, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  void info(Args&&... args) const {
    if constexpr (isCompiledIn(LogLevel::Info)) {
      emit(LogLevel::Info, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  void warn(Args&&... args) const {
    if constexpr (isCompiledIn(LogLevel::Warn)) {
      emit(LogLevel::Warn, std::forward<Args>(args)...);
    }
  }

  template <class... Args>
  void error(Args&&... args) const {
    if constexpr (isCompiledIn(LogLevel::Error)) {
      emit(LogLevel::Error, std::forward<Args>(args)...);
    }
  }

  class ScopedTimer {
   public:
    ScopedTimer(std::string module, std::string label, LogLevel level);

    /**
     * @brief Writes the elapsed time. A failure to write is swallowed: a
     *        journal must never take the program down.
     */
    ~ScopedTimer();

    ScopedTimer(ScopedTimer&& other) noexcept;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

   private:
    std::string module_;
    std::string label_;
    LogLevel level_;
    std::chrono::steady_clock::time_point begin_;
    bool active_{true};
  };

  [[nodiscard]] ScopedTimer scope(std::string label,
                                  LogLevel level = LogLevel::Info) const;

 private:
  template <class... Args>
  void emit(LogLevel candidate, Args&&... args) const {
    if (!shouldLog(candidate)) {
      return;
    }
    std::ostringstream stream;
    (stream << ... << std::forward<Args>(args));
    writeLine(candidate, stream.str());
  }

  void writeLine(LogLevel candidate, std::string_view body) const;

  std::string module_;
};

}  // namespace rtype::logging
