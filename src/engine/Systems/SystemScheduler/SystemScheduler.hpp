#pragma once

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>
#include "IClock.hpp"
#include "ISystem.hpp"
#include "SystemTime.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

/**
 * @brief Runs systems in the order they were added, and can measure how long
 * each one takes.
 *
 * The order is the one of the add() calls and never changes: every run() calls
 * the systems in that same order. add() must not be called from inside a
 * system.
 *
 * It reads no clock until measureWith() gives it one, so a scheduler nobody
 * measures runs as if the measure did not exist. Measuring allocates nothing
 * per run.
 */
class SystemScheduler {
 public:
  /**
   * @param name What takeSystemTimes() calls the system: a text that outlives
   * the scheduler, such as a constant. Leave it empty when nobody reads it.
   * @throws NullSystemException if system is null.
   */
  void add(std::unique_ptr<ISystem> system, std::string_view name = {});

  /** @brief Calls update() on every system, in the order they were added. */
  void run(ComponentRegistry& components, Duration elapsed);

  /** @returns The number of systems added. */
  [[nodiscard]] std::size_t size() const;

  /**
   * @brief Makes every later run() read @p clock around each system.
   * @param clock Must stay valid until the next call; nullptr stops measuring.
   */
  void measureWith(const IClock* clock);

  /**
   * @returns How long each system ran since the previous call, in the order
   * they were added, then starts counting again from zero. Every total is zero
   * while nothing measures.
   */
  [[nodiscard]] std::vector<SystemTime> takeSystemTimes();

 private:
  void runMeasured(ComponentRegistry& components, Duration elapsed);

  std::vector<std::unique_ptr<ISystem>> systems_;
  std::vector<SystemTime> times_;
  const IClock* clock_{nullptr};
};

}  // namespace rtype::engine
