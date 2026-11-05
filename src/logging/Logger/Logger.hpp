/**
 * @file Logger.hpp
 * @brief Pino/Nest-style structured logger, instance-based.
 *
 * Each class that logs owns a `Logger` instance whose name is supplied at
 * construction:
 *
 * ```cpp
 * class NetworkContext {
 *  private:
 *     rtype::logging::Logger logger_{"Network"};
 * };
 *
 * logger_.info("listening on port ", port);
 * logger_.warn("dropped a packet of ", size, " bytes");
 * auto timer = logger_.scope("load assets");
 * ```
 *
 * Output line format: `[YYYY-MM-DDTHH:MM:SS.mmmZ] [LEVEL] [Name] - body`,
 * with the time in UTC. Lines go to the sinks set by `Logger::setOutput()`:
 * a file (appended to, flushed after every line) and/or standard error.
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
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "ILogJournal.hpp"
#include "LogLevel.hpp"

namespace rtype::logging {

/**
 * @brief Where lines are written. Standard error is on by default so that a
 *        program that never calls Logger::setOutput() still shows its logs.
 */
struct LogOutput {
  bool toStderr{true};
  std::optional<std::filesystem::path> filePath;
};

class Logger {
 public:
  /**
   * @brief Build a logger bound to @p name, paid once at construction and
   *        reused for every subsequent call.
   */
  explicit Logger(std::string name);

  /**
   * @brief Builds a logger that writes through @p journal, which must outlive
   *        it, instead of the sinks of this copy of the library: what a plugin
   *        uses, with the journal it required at install. The static
   *        functions and scope() still use the state of this copy.
   */
  Logger(std::string name, const ILogJournal& journal);

  static LogLevel level();
  static void setLevel(LogLevel newLevel);
  static bool shouldLog(LogLevel candidate);

  /**
   * @return the level named @p text, compared without regard to case, or
   *         nothing when @p text names no level. Accepted names: trace,
   *         debug, info, warn (or warning), error, silent (or off, none).
   */
  static std::optional<LogLevel> parseLevel(std::string_view text);

  /**
   * @brief Replaces the sinks. The file is opened in append mode, before
   *        anything changes, and flushed after every line.
   * @throws LogFileOpenException when the file cannot be opened. The previous
   *         sinks are then kept.
   */
  static void setOutput(const LogOutput& output);

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
    ScopedTimer(std::string name, std::string label, LogLevel level);

    /**
     * @brief Writes the elapsed time. A failure to read the level or to
     *        write is swallowed: a journal must never take the program down.
     */
    ~ScopedTimer();

    ScopedTimer(ScopedTimer&& other) noexcept;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

   private:
    std::string name_;
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
    if (journal_ == nullptr ? !shouldLog(candidate)
                            : !journal_->accepts(candidate)) {
      return;
    }
    std::ostringstream stream;
    (stream << ... << std::forward<Args>(args));
    writeLine(candidate, stream.str());
  }

  void writeLine(LogLevel candidate, std::string_view body) const;

  std::string name_;
  const ILogJournal* journal_{nullptr};
};

}  // namespace rtype::logging
