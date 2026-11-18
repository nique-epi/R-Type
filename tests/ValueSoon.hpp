#pragma once

#include <chrono>
#include <optional>
#include <thread>

/** @brief How long valueSoon() sleeps between two reads. */
constexpr std::chrono::milliseconds VALUE_SOON_POLL_STEP{10};

/**
 * @returns The first value @p read gives, read every VALUE_SOON_POLL_STEP
 * until @p patience passed; nothing when it gave none by then. @p read
 * returns nothing while the value a test waits for has not come, as a
 * reading another thread publishes.
 */
template <typename Read>
auto valueSoon(std::chrono::steady_clock::duration patience, Read read)
    -> decltype(read()) {
  const auto deadline = std::chrono::steady_clock::now() + patience;
  while (std::chrono::steady_clock::now() < deadline) {
    if (auto value = read()) {
      return value;
    }
    std::this_thread::sleep_for(VALUE_SOON_POLL_STEP);
  }
  return std::nullopt;
}
