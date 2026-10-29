#pragma once

#include <gtest/gtest.h>
#include <span>
#include <vector>
#include "FrameLoop.hpp"
#include "SimulatedClock.hpp"
#include "TimeConstants.hpp"

/**
 * @brief A frame loop on a simulated clock, run over an application whose one
 * work records the time each pass gets.
 */
class FrameLoopFixture : public ::testing::Test {
 protected:
  /**
   * @brief Runs the loop: at each pass, the work records the time it gets,
   * moves the clock by the next duration of @p passDurations, and stops the
   * loop at the last one.
   */
  void runPasses(std::span<const rtype::engine::Duration> passDurations);

  /** @brief Stops the loop before it runs. */
  void stopBeforeRunning();

  /** @returns The time each pass got, in the order the passes ran. */
  [[nodiscard]] const std::vector<rtype::engine::Duration>& elapsedByPass()
      const;

 private:
  SimulatedClock clock_;
  rtype::client::FrameLoop loop_{clock_};
  std::vector<rtype::engine::Duration> elapsedByPass_;
};
