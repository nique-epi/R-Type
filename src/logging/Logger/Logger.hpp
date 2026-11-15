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
 * with the time of the call in UTC. Lines go to the sinks set by
 * `Logger::setOutput()`: a file (appended to, flushed after every line) and/or
 * standard error. A call only queues its line: one thread of the journal writes
 * the lines, in the order they were queued, so the thread that logs never waits
 * for the disk (see JournalWriter). When the queue is full, the new line is
 * dropped and the next line written says how many were lost. flushJournal()
 * waits for the lines queued before it; closeJournal() writes them and stops
 * the thread, which every program does before main() returns.
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
#include <functional>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "ILogJournal.hpp"
#include "LogFormat.hpp"
#include "LogLevel.hpp"
#include "LogRecord.hpp"

namespace rtype::logging {

/**
 * @brief Where lines are written. Standard error is on by default so that a
 *        program that never calls Logger::setOutput() still shows its logs.
 */
struct LogOutput {
  bool toStderr{true};
  std::optional<std::filesystem::path> filePath;
  /** @brief Colour on standard error even when it is no terminal:
   * `--log-color`, which the supervisor gives a game whose standard error it
   * relays. */
  bool forceColor{false};
  /** @brief How the file writes a line: text, or JSON Lines for a tool, with
   * `--log-format json`. Standard error always shows the text. */
  LogFormat fileFormat{LogFormat::Text};
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
   * @brief Replaces the sinks once the lines queued before the call are
   *        written to the old ones. The file is opened in append mode, before
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

/**
 * @brief Writes @p record as a line, to the sinks of this copy of the library,
 * without a level check: the caller already asked a journal or a logger which
 * levels are written.
 */
void writeLogLine(const LogRecord& record);

/**
 * @brief Hands every line this copy of the library writes to standard error to
 * @p standardErrorWriter, without its end, instead of std::cerr, from the lines
 * queued after the call on; an empty one writes them to std::cerr again. It
 * runs on the thread of the journal, under the lock of the sinks, so it must
 * never write to the journal itself. The admin console of the supervisor uses
 * it to keep the line being typed under the lines of the journal.
 */
void setStandardErrorWriter(
    std::function<void(std::string_view)> standardErrorWriter);

/**
 * @brief Queues @p line, without its end, for standard error, through the
 * writer of setStandardErrorWriter() when one is set: a line that does not come
 * from this journal, such as a line a game process wrote, in its place among
 * the lines of the journal. @p line may hold several lines, one end of line
 * between two.
 */
void writeStandardErrorLine(std::string_view line);

/** @brief Waits until every line queued before the call is written. Does
 * nothing on the thread of the journal. */
void flushJournal();

/** @brief Writes every queued line, then stops the thread of the journal; the
 * next line starts it again. Every program calls it before main() returns, so
 * no thread is left running while the statics are destroyed. */
void closeJournal();

/** @returns Whether this process writes its journal in colour on its own:
 * standard error is a terminal, and neither NO_COLOR nor RT_LOG_NO_COLOR is
 * set (Windows: never). */
[[nodiscard]] bool isStandardErrorColored();

/**
 * @returns The first name Logger::parseLevel() reads as @p level: trace,
 * debug, info, warn, error or silent.
 */
[[nodiscard]] std::string_view levelNameOf(LogLevel level);

}  // namespace rtype::logging
