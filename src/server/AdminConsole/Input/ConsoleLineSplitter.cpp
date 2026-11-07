#include "ConsoleLineSplitter.hpp"
#include <span>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "ConsoleChunk.hpp"

namespace rtype::server {

ConsoleChunk ConsoleLineSplitter::feed(std::span<const char> bytes) {
  ConsoleChunk chunk;
  for (const char character : bytes) {
    if (character == LINE_END) {
      endLine(chunk);
    } else {
      keep(character);
    }
  }
  return chunk;
}

ConsoleChunk ConsoleLineSplitter::finish() {
  ConsoleChunk chunk;
  if (!pending_.empty() || overlong_) {
    endLine(chunk);
  }
  return chunk;
}

void ConsoleLineSplitter::keep(char character) {
  if (overlong_) {
    return;
  }
  if (pending_.size() == MAXIMUM_CONSOLE_LINE_LENGTH) {
    overlong_ = true;
    pending_.clear();
    return;
  }
  pending_.push_back(character);
}

void ConsoleLineSplitter::endLine(ConsoleChunk& chunk) {
  if (overlong_) {
    ++chunk.droppedLines;
  } else {
    if (!pending_.empty() && pending_.back() == CARRIAGE_RETURN) {
      pending_.pop_back();
    }
    chunk.lines.push_back(std::move(pending_));
  }
  pending_.clear();
  overlong_ = false;
}

}  // namespace rtype::server
