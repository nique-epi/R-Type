#include "TimezoneOverride.hpp"

#if defined(_WIN32)
#include <memory>
#endif

#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>

namespace {

std::optional<std::string> readTimezone() {
#if defined(_WIN32)
  char* rawValue = nullptr;
  std::size_t length = 0;
  if (_dupenv_s(&rawValue, &length, "TZ") != 0) {
    return std::nullopt;
  }
  const std::unique_ptr<char, decltype(&std::free)> owned(rawValue, &std::free);
  if (owned == nullptr) {
    return std::nullopt;
  }
  return std::string(owned.get());
#else
  // NOLINTNEXTLINE(concurrency-mt-unsafe)
  const char* value = std::getenv("TZ");
  if (value == nullptr) {
    return std::nullopt;
  }
  return std::string(value);
#endif
}

void writeTimezone(const std::optional<std::string>& value) {
#if defined(_WIN32)
  _putenv_s("TZ", value.value_or("").c_str());
  _tzset();
#else
  if (value.has_value()) {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    setenv("TZ", value->c_str(), 1);
  } else {
    // NOLINTNEXTLINE(concurrency-mt-unsafe,misc-include-cleaner)
    unsetenv("TZ");
  }
  // NOLINTNEXTLINE(misc-include-cleaner)
  tzset();
#endif
}

}  // namespace

TimezoneOverride::TimezoneOverride(const std::string& value)
    : previous_(readTimezone()) {
  writeTimezone(value);
}

TimezoneOverride::~TimezoneOverride() { writeTimezone(previous_); }
