#include "ScriptedConsoleInput.hpp"
#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <utility>
#include <vector>

ScriptedConsoleInput::ScriptedConsoleInput(std::vector<std::string> chunks,
                                           bool endsAfterChunks)
    : chunks_(std::move(chunks)), endsAfterChunks_(endsAfterChunks) {}

std::optional<std::size_t> ScriptedConsoleInput::read(
    std::span<char> buffer, const std::stop_token& stop) {
  if (next_ < chunks_.size()) {
    const std::string& chunk = chunks_[next_];
    ++next_;
    std::ranges::copy(chunk, buffer.begin());
    return chunk.size();
  }
  if (endsAfterChunks_) {
    return std::nullopt;
  }
  std::mutex mutex;
  std::condition_variable_any woken;
  std::unique_lock lock{mutex};
  static_cast<void>(woken.wait(lock, stop, [] { return false; }));
  wokeOnStop_.store(true);
  return std::nullopt;
}

bool ScriptedConsoleInput::wokeOnStop() const { return wokeOnStop_.load(); }
