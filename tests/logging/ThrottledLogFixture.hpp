#pragma once

#include <gtest/gtest.h>
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>
#include "DebugJournal.hpp"
#include "ThrottledLog.hpp"
#include "ThrottledLogTestConstants.hpp"

namespace rtype::logging::testing {

/** @brief A throttled log, with the log written to a journal the test reads. */
class ThrottledLogFixture : public ::testing::Test {
 protected:
  /** @brief Has the log count one event that happened at @p now. */
  void eventAt(std::chrono::nanoseconds now);

  /** @brief Has the log count one event that happened at @p now, written at
   * the error level when it gets a line. */
  void errorAt(std::chrono::nanoseconds now);

  /** @returns The lines the throttled log wrote, oldest first. */
  [[nodiscard]] std::vector<std::string> lines() const;

  /** @returns The end of a line that stands for @p count events. */
  [[nodiscard]] static std::string countEnding(std::size_t count);

 private:
  DebugJournal journal_;
  ThrottledLog log_{std::string{THROTTLED_LOGGER_NAME}, THROTTLED_INTERVAL};
};

}  // namespace rtype::logging::testing
