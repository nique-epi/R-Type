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
#include <iomanip>
#include <iostream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "LoggingConstants.hpp"

namespace rtype::logging {

namespace {

std::mutex& outputMutex() {
  static std::mutex mutex;
  return mutex;
}

std::string toLower(std::string_view input) {
  std::string out(input);
  std::ranges::transform(out, out.begin(), [](unsigned char character) {
    return std::tolower(character);
  });
  return out;
}

LogLevel parseLevel(std::string_view text, LogLevel fallback) {
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
  return fallback;
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
  return parseLevel(*value, LogLevel::Info);
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
  std::tm broken{};
#if defined(_WIN32)
  localtime_s(&broken, &timeValue);
#else
  (void)localtime_r(&timeValue, &broken);
#endif
  std::ostringstream out;
  out << std::put_time(&broken, TIMESTAMP_FORMAT) << '.'
      << std::setw(MILLISECOND_DIGITS) << std::setfill('0')
      << elapsedMilliseconds;
  return out.str();
}

}  // namespace

Logger::Logger(std::string module) : module_(std::move(module)) {}

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

void Logger::writeLine(LogLevel candidate, std::string_view body) const {
  const std::string_view color =
      colorEnabled() ? ansiForLevel(candidate) : std::string_view{};
  const std::string_view reset =
      colorEnabled() ? ANSI_RESET : std::string_view{};
  const std::string timestamp = formatTimestamp();

  const std::lock_guard<std::mutex> lock(outputMutex());
  std::cerr << color << '[' << timestamp << "] [" << labelForLevel(candidate)
            << "] [" << module_ << "] - " << body << reset << '\n';
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

    const std::string_view color =
        colorEnabled() ? ansiForLevel(level_) : std::string_view{};
    const std::string_view reset =
        colorEnabled() ? ANSI_RESET : std::string_view{};
    const std::string timestamp = formatTimestamp();

    const std::lock_guard<std::mutex> lock(outputMutex());
    std::cerr << color << '[' << timestamp << "] [" << labelForLevel(level_)
              << "] [" << module_ << "] - " << out.str() << reset << '\n';
  } catch (...) {  // NOLINT(bugprone-empty-catch)
  }
}

}  // namespace rtype::logging
