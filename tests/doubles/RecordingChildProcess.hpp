#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include "IChildProcess.hpp"
#include "ProcessExit.hpp"
#include "RecordingChildState.hpp"

/** @brief A child that runs no process: it ends when its test says so, or when
 * it is terminated. */
class RecordingChildProcess final : public rtype::server::IChildProcess {
 public:
  RecordingChildProcess(std::int64_t processId,
                        std::shared_ptr<RecordingChildState> state);

  void terminate() override;
  [[nodiscard]] std::optional<rtype::server::ProcessExit> exited() override;
  [[nodiscard]] std::int64_t processId() const override;

 private:
  std::int64_t processId_;
  std::shared_ptr<RecordingChildState> state_;
};
