#include "Logger.hpp"

#if defined(_WIN32)
#include <memory>
#else
#include <unistd.h>
#endif

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "LoggingConstants.hpp"
#include "LoggingException.hpp"

namespace rtype::logging {

namespace {

struct Sinks {
  std::mutex mutex;
  std::ofstream file;
  bool toStderr{true};
};

Sinks& sinks() {
  static Sinks instance;
  return instance;
}

std::string toLower(std::string_view input) {
  std::string out(input);
  std::ranges::transform(out, out.begin(), [](unsigned char character) {
    return std::tolower(character);
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

std::string formatTimestamp() {
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;
  using std::chrono::system_clock;
  using std::chrono::time_point_cast;
  const auto now = system_clock::now();
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

void emitLine(LogLevel level, std::string_view module, std::string_view body) {
  Sinks& destination = sinks();
  const std::lock_guard<std::mutex> lock(destination.mutex);
  std::ostringstream line;
  line << '[' << formatTimestamp() << "] [" << labelForLevel(level) << "] ["
       << module << "] - " << body;
  const std::string text = line.str();
  if (destination.file.is_open()) {
    destination.file << text << '\n';
    destination.file.flush();
  }
  if (destination.toStderr) {
    if (colorEnabled()) {
      std::cerr << ansiForLevel(level) << text << ANSI_RESET << '\n';
    } else {
      std::cerr << text << '\n';
    }
  }
}

}  // namespace

Logger::Logger(std::string module) : module_(std::move(module)) {}

std::optional<LogLevel> Logger::parseLevel(std::string_view text) {
  const std::string lower = toLower(text);
  if (lower == TRACE_NAME) {
    return LogLevel::Trace;
  }
  if (lower == DEBUG_NAME) {
    return LogLevel::Debug;
  }
  if (lower == INFO_NAME) {
    return LogLevel::Info;
  }
  if (lower == WARN_NAME || lower == WARNING_NAME) {
    return LogLevel::Warn;
  }
  if (lower == ERROR_NAME) {
    return LogLevel::Error;
  }
  if (lower == SILENT_NAME || lower == OFF_NAME || lower == NONE_NAME) {
    return LogLevel::Silent;
  }
  return std::nullopt;
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
      throw LogFileOpenException(output.filePath->string());
    }
  }
  Sinks& destination = sinks();
  const std::lock_guard<std::mutex> lock(destination.mutex);
  destination.file.close();
  destination.file = std::move(file);
  destination.toStderr = output.toStderr;
}

void Logger::writeLine(LogLevel candidate, std::string_view body) const {
  emitLine(candidate, module_, body);
}

Logger::ScopedTimer Logger::scope(std::string label, LogLevel level) const {
  return {module_, std::move(label), level};
}

Logger::ScopedTimer::ScopedTimer(std::string module, std::string label,
                                 LogLevel level)
    : module_(std::move(module)),
      label_(std::move(label)),
      level_(level),
      begin_(std::chrono::steady_clock::now()) {}

Logger::ScopedTimer::ScopedTimer(ScopedTimer&& other) noexcept
    : module_(std::move(other.module_)),
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
  if (!Logger::shouldLog(level_)) {
    return;
  }
  try {
    const auto elapsed = std::chrono::steady_clock::now() - begin_;
    const auto microseconds =
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    const double milliseconds =
        static_cast<double>(microseconds) / MICROSECONDS_PER_MILLISECOND;
    std::ostringstream out;
    out << label_ << " took " << std::fixed
        << std::setprecision(DURATION_DECIMALS) << milliseconds << " ms";
    emitLine(level_, module_, out.str());
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

}  // namespace rtype::logging
