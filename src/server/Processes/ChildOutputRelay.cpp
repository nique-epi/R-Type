#include "ChildOutputRelay.hpp"
#include <array>
#include <boost/asio/buffer.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <utility>
#include <vector>
#include "Logger.hpp"
#include "ProcessConstants.hpp"

namespace rtype::server {

namespace {

/** @brief Writes @p lines to standard error as one text, one end of line
 * between two, so a burst of lines takes the lock of the journal and reaches
 * the terminal once. */
void writeLines(const std::vector<std::string>& lines) {
  if (lines.empty()) {
    return;
  }
  std::string text = lines.front();
  for (const std::string& line : std::span{lines}.subspan(1)) {
    text += CHILD_LINE_END;
    text += line;
  }
  logging::writeStandardErrorLine(text);
}

}  // namespace

ChildOutputRelay::ChildOutputRelay(boost::asio::readable_pipe pipe)
    : pipe_(std::move(pipe)), reader_([this] { run(); }) {}

void ChildOutputRelay::run() {
  std::array<char, CHILD_OUTPUT_READ_SIZE> buffer{};
  boost::system::error_code error;
  std::size_t count = pipe_.read_some(boost::asio::buffer(buffer), error);
  while (!error || error == boost::asio::error::interrupted) {
    writeLines(lines_.feed(std::span<const char>{buffer}.first(count)));
    count = pipe_.read_some(boost::asio::buffer(buffer), error);
  }
  if (const std::optional<std::string> last = lines_.finish();
      last.has_value()) {
    logging::writeStandardErrorLine(*last);
  }
}

}  // namespace rtype::server
