#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include "ISystem.hpp"
#include "TimeConstants.hpp"

namespace rtype::engine {

class ComponentRegistry;

/**
 * @brief Runs systems in the order they were added.
 *
 * The order is the one of the add() calls and never changes: every run() calls
 * the systems in that same order. add() must not be called from inside a
 * system.
 */
class SystemScheduler {
 public:
  /**
   * @throws NullSystemException if system is null.
   */
  void add(std::unique_ptr<ISystem> system);

  /** @brief Calls update() on every system, in the order they were added. */
  void run(ComponentRegistry& components, Duration elapsed);

  /** @returns The number of systems added. */
  std::size_t size() const;

 private:
  std::vector<std::unique_ptr<ISystem>> systems_;
};

}  // namespace rtype::engine
