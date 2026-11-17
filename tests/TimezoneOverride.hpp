#pragma once

#include <optional>
#include <string>

/** @brief Sets the TZ variable of the process to a value for its life, then
 * puts the previous value back, or removes it when there was none. */
class TimezoneOverride {
 public:
  explicit TimezoneOverride(const std::string& value);
  ~TimezoneOverride();

  TimezoneOverride(const TimezoneOverride&) = delete;
  TimezoneOverride& operator=(const TimezoneOverride&) = delete;
  TimezoneOverride(TimezoneOverride&&) = delete;
  TimezoneOverride& operator=(TimezoneOverride&&) = delete;

 private:
  std::optional<std::string> previous_;
};
