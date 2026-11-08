#include "RecordingConsoleOutput.hpp"
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

void RecordingConsoleOutput::write(std::string_view line) {
  const std::scoped_lock lock{mutex_};
  lines_.emplace_back(line);
}

std::vector<std::string> RecordingConsoleOutput::lines() const {
  const std::scoped_lock lock{mutex_};
  return lines_;
}
