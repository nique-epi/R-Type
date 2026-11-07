#include "ConsoleReader.hpp"
#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <stop_token>
#include <string>
#include <utility>
#include "AdminConsoleConstants.hpp"
#include "BoundedQueue.hpp"
#include "ConsoleChunk.hpp"
#include "IConsoleInput.hpp"

namespace rtype::server {

ConsoleReader::ConsoleReader(IConsoleInput& input,
                             concurrency::BoundedQueue<std::string>& lines)
    : input_(&input),
      lines_(&lines),
      logger_(std::string{ADMIN_CONSOLE_LOGGER_NAME}),
      console_([this](const std::stop_token& stop) { run(stop); }) {}

void ConsoleReader::run(const std::stop_token& stop) {
  std::array<char, CONSOLE_READ_SIZE> buffer{};
  std::optional<std::size_t> received = input_->read(buffer, stop);
  while (received.has_value()) {
    handOver(splitter_.feed(std::span<const char>{buffer}.first(*received)));
    received = input_->read(buffer, stop);
  }
  if (stop.stop_requested()) {
    return;
  }
  handOver(splitter_.finish());
  logger_.info("the console input ended: no command is read from it any more");
}

void ConsoleReader::handOver(ConsoleChunk chunk) {
  for (std::string& line : chunk.lines) {
    if (lines_->push(std::move(line))) {
      logger_.warn("a console line was dropped: ", CONSOLE_LINE_CAPACITY,
                   " lines were already waiting");
    }
  }
  if (chunk.droppedLines > 0) {
    logger_.warn(chunk.droppedLines, " console lines longer than ",
                 MAXIMUM_CONSOLE_LINE_LENGTH, " bytes dropped");
  }
}

}  // namespace rtype::server
