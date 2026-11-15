#include "Logger.hpp"

#if defined(_WIN32)
#include <memory>
#else
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "ILogJournal.hpp"
#include "JournalLine.hpp"
#include "JournalLineParts.hpp"
#include "JournalWriter.hpp"
#include "LogEvent.hpp"
#include "LogFormat.hpp"
#include "LogLevel.hpp"
#include "LogRecord.hpp"
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"
#include "PathText.hpp"
#include "QueuedLine.hpp"

namespace rtype::logging {

namespace {

struct Sinks {
  std::mutex mutex;
  std::ofstream file;
  bool toStderr{true};
  bool forceColor{false};
  LogFormat fileFormat{LogFormat::Text};
  std::function<void(std::string_view)> standardError;
};

/** @returns Whether the journal file of this copy of the library is written as
 * JSON, the only place the fields of an event are written: an event formats
 * its fields only then, or when it goes through a journal. */
std::atomic<bool>& fileWritesJson() {
  static std::atomic<bool> json{false};
  return json;
}

Sinks& sinks() {
  static Sinks instance;
  return instance;
}

struct LevelName {
  std::string_view name;
  LogLevel level;
};

constexpr std::array levelNames{
    LevelName{.name = TRACE_NAME, .level = LogLevel::Trace},
    LevelName{.name = DEBUG_NAME, .level = LogLevel::Debug},
    LevelName{.name = INFO_NAME, .level = LogLevel::Info},
    LevelName{.name = WARN_NAME, .level = LogLevel::Warn},
    LevelName{.name = WARNING_NAME, .level = LogLevel::Warn},
    LevelName{.name = ERROR_NAME, .level = LogLevel::Error},
    LevelName{.name = SILENT_NAME, .level = LogLevel::Silent},
    LevelName{.name = OFF_NAME, .level = LogLevel::Silent},
    LevelName{.name = NONE_NAME, .level = LogLevel::Silent},
};

std::string toLower(std::string_view input) {
  std::string out(input);
  std::ranges::transform(out, out.begin(), [](unsigned char character) {
    return static_cast<char>(std::tolower(character));
  });
  return out;
}

std::optional<std::string> readEnvironment(std::string_view name) {
  const std::string terminatedName(name);
#if defined(_WIN32)
  char* rawValue = nullptr;
  std::size_t length = 0;
  if (_dupenv_s(&rawValue, &length, terminatedName.c_str()) != 0) {
    return std::nullopt;
  }
  const std::unique_ptr<char, decltype(&std::free)> owned(rawValue, &std::free);
  if (owned == nullptr) {
    return std::nullopt;
  }
  return std::string(owned.get());
#else
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* value = std::getenv(terminatedName.c_str());
  if (value == nullptr) {
    return std::nullopt;
  }
  return std::string(value);
#endif
}

LogLevel initialLevel() {
  const std::optional<std::string> value =
      readEnvironment(LEVEL_ENVIRONMENT_VARIABLE);
  if (!value.has_value()) {
    return LogLevel::Info;
  }
  return Logger::parseLevel(*value).value_or(LogLevel::Info);
}

std::atomic<LogLevel>& currentLevel() {
  static std::atomic<LogLevel> level{initialLevel()};
  return level;
}

bool computeColorEnabled() {
  if (readEnvironment(NO_COLOR_ENVIRONMENT_VARIABLE).has_value() ||
      readEnvironment(PROJECT_NO_COLOR_ENVIRONMENT_VARIABLE).has_value()) {
    return false;
  }
#if defined(_WIN32)
  return false;
#else
  return ::isatty(STDERR_FILENO) != 0;
#endif
}

bool colorEnabled() {
  static const bool enabled = computeColorEnabled();
  return enabled;
}

std::string_view ansiForLevel(LogLevel level) {
  switch (level) {
    case LogLevel::Trace:
      return ANSI_GRAY;
    case LogLevel::Debug:
      return ANSI_CYAN;
    case LogLevel::Info:
      return ANSI_GREEN;
    case LogLevel::Warn:
      return ANSI_YELLOW;
    case LogLevel::Error:
      return ANSI_RED;
    case LogLevel::Silent:
      return {};
  }
  return {};
}

std::string_view labelForLevel(LogLevel level) {
  switch (level) {
    case LogLevel::Trace:
      return TRACE_LABEL;
    case LogLevel::Debug:
      return DEBUG_LABEL;
    case LogLevel::Info:
      return INFO_LABEL;
    case LogLevel::Warn:
      return WARN_LABEL;
    case LogLevel::Error:
      return ERROR_LABEL;
    case LogLevel::Silent:
      return SILENT_LABEL;
  }
  return UNKNOWN_LABEL;
}

std::string formatTimestamp(std::chrono::system_clock::time_point now) {
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::system_clock;
  using std::chrono::time_point_cast;
  const auto seconds = time_point_cast<std::chrono::seconds>(now);
  const auto elapsedMilliseconds =
      duration_cast<milliseconds>(now - seconds).count();
  const std::time_t timeValue = system_clock::to_time_t(now);
  std::tm universalTime{};
#if defined(_WIN32)
  (void)gmtime_s(&universalTime, &timeValue);
#else
  (void)gmtime_r(&timeValue, &universalTime);
#endif
  std::ostringstream out;
  out << std::put_time(&universalTime, TIMESTAMP_FORMAT) << '.'
      << std::setw(MILLISECOND_DIGITS) << std::setfill('0')
      << elapsedMilliseconds << 'Z';
  return out.str();
}

/** @brief Writes @p line to standard error through the writer of
 * @p destination, or to std::cerr; the caller holds the lock of
 * @p destination. */
void writeToStandardError(const Sinks& destination, std::string_view line) {
  if (destination.standardError) {
    destination.standardError(line);
    return;
  }
  std::cerr << line << '\n';
}

/** @returns @p text as standard error shows it: between the colour of
 * @p level and ANSI_RESET when colour is on. */
std::string standardErrorText(const Sinks& destination, LogLevel level,
                              const std::string& text) {
  if (!destination.forceColor && !colorEnabled()) {
    return text;
  }
  return std::string{ansiForLevel(level)} + text + std::string{ANSI_RESET};
}

/** @returns The text line of @p record, called at @p timeText. */
std::string textLineOf(const LogRecord& record, std::string_view timeText) {
  std::ostringstream stream;
  stream << '[' << timeText << "] [" << labelForLevel(record.level) << "] ["
         << record.loggerName << "] - " << record.body;
  return stream.str();
}

/**
 * @brief Writes @p record, called at @p time, to the file, flushed, and to
 * standard error; the caller holds the lock of @p destination. The text of a
 * line holds its message only; the JSON of the file adds the fields of an
 * event.
 */
void writeJournalLine(Sinks& destination, const LogRecord& record,
                      std::chrono::system_clock::time_point time) {
  const std::string timeText = formatTimestamp(time);
  const bool jsonFile = destination.fileFormat == LogFormat::Json;
  const std::string text = !jsonFile || destination.toStderr
                               ? textLineOf(record, timeText)
                               : std::string{};
  if (destination.file.is_open()) {
    destination.file << (jsonFile ? jsonLineOf(JournalLineParts{
                                        .time = timeText,
                                        .level = levelNameOf(record.level),
                                        .logger = record.loggerName,
                                        .message = record.body,
                                        .fields = record.fields})
                                  : text)
                     << '\n';
    destination.file.flush();
  }
  if (destination.toStderr) {
    writeToStandardError(destination,
                         standardErrorText(destination, record.level, text));
  }
}

/** @brief Writes, under LOGGING_LOGGER_NAME, how many lines the full queue
 * dropped before @p line; the caller holds the lock of @p destination. */
void writeLostLines(Sinks& destination, const QueuedLine& line) {
  const std::string body =
      std::to_string(line.lostBefore) + std::string{LOST_LINES_TEXT};
  writeJournalLine(destination,
                   LogRecord{.level = LogLevel::Warn,
                             .loggerName = LOGGING_LOGGER_NAME,
                             .body = body,
                             .fields = {}},
                   line.time);
}

/** @returns The size @p digits writes in decimal; nothing when it is empty,
 * holds another character, or is longer than any size emitLine() writes. */
std::optional<std::size_t> sizeIn(std::string_view digits) {
  if (digits.empty() || digits.size() > MESSAGE_SIZE_MAXIMUM_DIGITS) {
    return std::nullopt;
  }
  std::size_t size = 0;
  for (const char digit : digits) {
    if (digit < '0' || digit > '9') {
      return std::nullopt;
    }
    size = (size * DECIMAL_BASE) + static_cast<std::size_t>(digit - '0');
  }
  return size;
}

/**
 * @returns The record @p line carries after its kind: the size of the message,
 * MESSAGE_SIZE_END, the message, then the fields of an event. A text that does
 * not read so, which emitLine() never queues, is taken as a message alone.
 */
LogRecord recordOf(const QueuedLine& line) {
  const std::string_view text = line.text.substr(1);
  const LogRecord messageAlone{.level = line.level,
                               .loggerName = line.loggerName,
                               .body = text,
                               .fields = {}};
  const std::size_t sizeEnd = text.find(MESSAGE_SIZE_END);
  if (sizeEnd == std::string_view::npos) {
    return messageAlone;
  }
  const std::optional<std::size_t> messageSize =
      sizeIn(text.substr(0, sizeEnd));
  if (!messageSize.has_value() || *messageSize > text.size() - sizeEnd - 1) {
    return messageAlone;
  }
  const std::string_view rest = text.substr(sizeEnd + 1);
  return LogRecord{.level = line.level,
                   .loggerName = line.loggerName,
                   .body = rest.substr(0, *messageSize),
                   .fields = rest.substr(*messageSize)};
}

/**
 * @brief The destination of the JournalWriter, on the thread of the journal:
 * writes @p line where the sinks say, after the count of the lines lost before
 * it. A failure to write is swallowed: a journal must never take the program
 * down.
 */
void writeQueuedLine(const QueuedLine& line) {
  try {
    Sinks& destination = sinks();
    const std::lock_guard<std::mutex> lock(destination.mutex);
    if (line.lostBefore != 0) {
      writeLostLines(destination, line);
    }
    if (line.text.empty()) {
      return;
    }
    if (line.text.front() == STANDARD_ERROR_LINE_KIND) {
      writeToStandardError(destination, line.text.substr(1));
      return;
    }
    writeJournalLine(destination, recordOf(line), line.time);
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

/** @returns The writer of this copy of the library. The sinks it writes to are
 * built first, so they are destroyed after it, once it wrote every line. */
JournalWriter& writer() {
  sinks();
  static JournalWriter instance{&writeQueuedLine};
  return instance;
}

/** @brief Queues @p record for the thread that writes the journal, as
 * recordOf() reads it back. */
void emitLine(const LogRecord& record) {
  std::string text{JOURNAL_LINE_KIND};
  text += std::to_string(record.body.size());
  text += MESSAGE_SIZE_END;
  text += record.body;
  text += record.fields;
  writer().queue(record.level, record.loggerName, text);
}

/** @brief Writes @p record through @p journal, or to the sinks of this copy of
 * the library when there is none. */
void writeThrough(const ILogJournal* journal, const LogRecord& record) {
  if (journal != nullptr) {
    journal->write(record);
    return;
  }
  emitLine(record);
}

}  // namespace

Logger::Logger(std::string name) : name_(std::move(name)) {}

Logger::Logger(std::string name, const ILogJournal& journal)
    : name_(std::move(name)), journal_(&journal) {}

std::optional<LogLevel> Logger::parseLevel(std::string_view text) {
  const std::string lower = toLower(text);
  for (const LevelName& levelName : levelNames) {
    if (levelName.name == lower) {
      return levelName.level;
    }
  }
  return std::nullopt;
}

std::string_view levelNameOf(LogLevel level) {
  for (const LevelName& levelName : levelNames) {
    if (levelName.level == level) {
      return levelName.name;
    }
  }
  return INFO_NAME;
}

LogLevel Logger::level() {
  return currentLevel().load(std::memory_order_acquire);
}

void Logger::setLevel(LogLevel newLevel) {
  currentLevel().store(newLevel, std::memory_order_release);
}

bool Logger::shouldLog(LogLevel candidate) {
  return static_cast<std::uint8_t>(candidate) >=
         static_cast<std::uint8_t>(level());
}

void Logger::setOutput(const LogOutput& output) {
  std::ofstream file;
  if (output.filePath.has_value()) {
    file.open(*output.filePath, std::ios::app);
    if (!file.is_open()) {
      throw LogFileOpenException(genericText(*output.filePath));
    }
  }
  writer().flush();
  fileWritesJson().store(file.is_open() && output.fileFormat == LogFormat::Json,
                         std::memory_order_release);
  Sinks& destination = sinks();
  const std::lock_guard<std::mutex> lock(destination.mutex);
  destination.file.close();
  destination.file = std::move(file);
  destination.toStderr = output.toStderr;
  destination.forceColor = output.forceColor;
  destination.fileFormat = output.fileFormat;
}

void writeLogLine(const LogRecord& record) { emitLine(record); }

void setStandardErrorWriter(
    std::function<void(std::string_view)> standardErrorWriter) {
  writer().flush();
  Sinks& destination = sinks();
  const std::lock_guard<std::mutex> lock(destination.mutex);
  destination.standardError = std::move(standardErrorWriter);
}

void writeStandardErrorLine(std::string_view line) {
  std::string text{STANDARD_ERROR_LINE_KIND};
  text += line;
  writer().queue(LogLevel::Info, {}, text);
}

void flushJournal() { writer().flush(); }

void closeJournal() { writer().close(); }

bool isStandardErrorColored() { return colorEnabled(); }

void Logger::writeLine(LogLevel candidate, std::string_view body) const {
  writeThrough(
      journal_,
      LogRecord{
          .level = candidate, .loggerName = name_, .body = body, .fields = {}});
}

void Logger::event(const LogEvent& event) const {
  if (!isCompiledIn(event.level) ||
      (journal_ == nullptr ? !shouldLog(event.level)
                           : !journal_->accepts(event.level))) {
    return;
  }
  const bool fieldsAreRead =
      journal_ != nullptr || fileWritesJson().load(std::memory_order_acquire);
  const std::string fields = fieldsAreRead ? eventFieldsText(event) : "";
  writeThrough(journal_, LogRecord{.level = event.level,
                                   .loggerName = name_,
                                   .body = event.message,
                                   .fields = fields});
}

Logger::ScopedTimer Logger::scope(std::string label, LogLevel level) const {
  return {name_, std::move(label), level};
}

Logger::ScopedTimer::ScopedTimer(std::string name, std::string label,
                                 LogLevel level)
    : name_(std::move(name)),
      label_(std::move(label)),
      level_(level),
      begin_(std::chrono::steady_clock::now()) {}

Logger::ScopedTimer::ScopedTimer(ScopedTimer&& other) noexcept
    : name_(std::move(other.name_)),
      label_(std::move(other.label_)),
      level_(other.level_),
      begin_(other.begin_),
      active_(other.active_) {
  other.active_ = false;
}

Logger::ScopedTimer::~ScopedTimer() {
  if (!active_) {
    return;
  }
  try {
    if (!Logger::shouldLog(level_)) {
      return;
    }
    const auto elapsed = std::chrono::steady_clock::now() - begin_;
    const auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    const double milliseconds =
        static_cast<double>(microseconds) / MICROSECONDS_PER_MILLISECOND;
    std::ostringstream out;
    out << label_ << " took " << std::fixed
        << std::setprecision(DURATION_DECIMALS) << milliseconds << " ms";
    const std::string body = out.str();
    emitLine(LogRecord{
        .level = level_, .loggerName = name_, .body = body, .fields = {}});
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

}  // namespace rtype::logging
