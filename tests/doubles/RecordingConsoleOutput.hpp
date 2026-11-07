#pragma once

#include <mutex>
#include <string>
#include <string_view>
#include <vector>
#include "IConsoleOutput.hpp"

/** @brief An output that keeps every line, in order, and writes none. Safe
 * from any thread: a supervisor runs its ticks on its network thread while
 * the test reads. */
class RecordingConsoleOutput final : public rtype::server::IConsoleOutput {
 public:
  void write(std::string_view line) override;

  /** @returns A copy of the lines written so far, oldest first. */
  [[nodiscard]] std::vector<std::string> lines() const;

 private:
  mutable std::mutex mutex_;
  std::vector<std::string> lines_;
};
