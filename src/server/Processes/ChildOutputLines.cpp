#include "ChildOutputLines.hpp"
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "ProcessConstants.hpp"

namespace rtype::server {

std::vector<std::string> ChildOutputLines::feed(std::span<const char> bytes) {
  std::vector<std::string> lines;
  for (const char character : bytes) {
    if (character == CHILD_LINE_END) {
      endLine(lines);
      continue;
    }
    pending_.push_back(character);
    if (pending_.size() == MAXIMUM_CHILD_LINE_LENGTH) {
      endLine(lines);
    }
  }
  return lines;
}

std::optional<std::string> ChildOutputLines::finish() {
  if (pending_.empty()) {
    return std::nullopt;
  }
  std::string last = std::move(pending_);
  pending_.clear();
  return last;
}

void ChildOutputLines::endLine(std::vector<std::string>& lines) {
  if (pending_.ends_with(CHILD_CARRIAGE_RETURN)) {
    pending_.pop_back();
  }
  lines.push_back(std::move(pending_));
  pending_.clear();
}

}  // namespace rtype::server
