#include "StageWorkFixtures.hpp"
#include <string>
#include <utility>
#include <vector>
#include "StageWork.hpp"
#include "TimeConstants.hpp"

rtype::engine::StageWork recordingWork(std::vector<std::string>& ran,
                                       std::string line) {
  return [&ran, recordedLine = std::move(line)](
             [[maybe_unused]] rtype::engine::Duration elapsed) {
    ran.push_back(recordedLine);
  };
}

rtype::engine::StageWork elapsedRecordingWork(
    std::vector<rtype::engine::Duration>& elapsedTimes) {
  return [&elapsedTimes](rtype::engine::Duration elapsed) {
    elapsedTimes.push_back(elapsed);
  };
}
