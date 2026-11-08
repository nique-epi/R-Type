#include "RecordingTerminal.hpp"
#include <mutex>
#include <string>
#include <string_view>

RecordingTerminal::RecordingTerminal(bool isTerminal)
    : isTerminal_(isTerminal) {}

bool RecordingTerminal::enterTypingMode() {
  const std::lock_guard<std::mutex> lock(mutex_);
  typing_ = isTerminal_;
  return typing_;
}

void RecordingTerminal::leaveTypingMode() {
  const std::lock_guard<std::mutex> lock(mutex_);
  typing_ = false;
}

void RecordingTerminal::write(std::string_view text) {
  const std::lock_guard<std::mutex> lock(mutex_);
  written_ += text;
}

std::string RecordingTerminal::written() const {
  const std::lock_guard<std::mutex> lock(mutex_);
  return written_;
}

bool RecordingTerminal::inTypingMode() const {
  const std::lock_guard<std::mutex> lock(mutex_);
  return typing_;
}
