#include "RecordingProcessLauncher.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include "IChildProcess.hpp"
#include "ProcessCommand.hpp"
#include "ProcessExit.hpp"
#include "RecordingChildProcess.hpp"
#include "RecordingChildState.hpp"
#include "RecordingProcessLauncherConstants.hpp"
#include "ServerException.hpp"

std::unique_ptr<rtype::server::IChildProcess> RecordingProcessLauncher::launch(
    rtype::server::ProcessCommand command) {
  if (failNext_) {
    failNext_ = false;
    throw rtype::server::ProcessLaunchException(command.program.string(),
                                                "refused by the test");
  }
  auto state = std::make_shared<RecordingChildState>();
  states_.push_back(state);
  const auto processId =
      FIRST_PROCESS_ID + static_cast<std::int64_t>(commands_.size());
  commands_.push_back(std::move(command));
  return std::make_unique<RecordingChildProcess>(processId, std::move(state));
}

const std::vector<rtype::server::ProcessCommand>&
RecordingProcessLauncher::commands() const {
  return commands_;
}

void RecordingProcessLauncher::exitChild(std::size_t index, int code) {
  states_.at(index)->exit = rtype::server::ProcessExit{.code = code};
}

bool RecordingProcessLauncher::childWasTerminated(std::size_t index) const {
  return states_.at(index)->terminated;
}

void RecordingProcessLauncher::failNextLaunch() { failNext_ = true; }
