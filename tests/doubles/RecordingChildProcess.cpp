#include "RecordingChildProcess.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include "ProcessExit.hpp"
#include "RecordingChildState.hpp"
#include "RecordingProcessLauncherConstants.hpp"

RecordingChildProcess::RecordingChildProcess(
    std::int64_t processId, std::shared_ptr<RecordingChildState> state)
    : processId_(processId), state_(std::move(state)) {}

void RecordingChildProcess::terminate() {
  state_->terminated = true;
  if (!state_->exit.has_value()) {
    state_->exit = rtype::server::ProcessExit{.code = KILLED_EXIT_CODE};
  }
}

std::optional<rtype::server::ProcessExit> RecordingChildProcess::exited() {
  return state_->exit;
}

std::int64_t RecordingChildProcess::processId() const { return processId_; }
