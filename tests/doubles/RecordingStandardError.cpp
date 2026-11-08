#include "RecordingStandardError.hpp"
#include <mutex>
#include <string>
#include <string_view>
#include <vector>
#include "Logger.hpp"

RecordingStandardError::RecordingStandardError() {
  rtype::logging::setStandardErrorWriter([this](std::string_view line) {
    const std::lock_guard<std::mutex> lock(mutex_);
    lines_.emplace_back(line);
  });
}

RecordingStandardError::~RecordingStandardError() {
  rtype::logging::setStandardErrorWriter({});
}

std::vector<std::string> RecordingStandardError::lines() const {
  const std::lock_guard<std::mutex> lock(mutex_);
  return lines_;
}
