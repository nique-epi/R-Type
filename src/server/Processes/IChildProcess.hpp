#pragma once

#include <cstdint>
#include <optional>
#include "ProcessExit.hpp"

namespace rtype::server {

/**
 * @brief A process the server started. Destroying it kills the process if it
 * still runs, so an owner that loses track of a child never leaves it behind.
 */
class IChildProcess {
 public:
  virtual ~IChildProcess() = default;

  /** @brief Kills the process at once, without letting it clean up. Does
   * nothing when it already ended. */
  virtual void terminate() = 0;

  /**
   * @returns How the process ended, or nothing while it still runs. Never
   * blocks. Once it returned a value it returns the same value.
   */
  [[nodiscard]] virtual std::optional<ProcessExit> exited() = 0;

  /** @returns The identifier the system gave the process. */
  [[nodiscard]] virtual std::int64_t processId() const = 0;
};

}  // namespace rtype::server
