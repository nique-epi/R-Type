#include "ChildOutputRelay.hpp"
#include <array>
#include <boost/asio/buffer.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/system/error_code.hpp>
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

void writeLines(const std::vector<std::string>& lines) {
  for (const std::string& line : lines) {
    logging::writeStandardErrorLine(line);
  }
}

}  // namespace

ChildOutputRelay::ChildOutputRelay(boost::asio::readable_pipe pipe)
    : pipe_(std::move(pipe)), reader_([this] { run(); }) {}

void ChildOutputRelay::run() {
  std::array<char, CHILD_OUTPUT_READ_SIZE> buffer{};
  boost::system::error_code error;
  std::size_t count = pipe_.read_some(boost::asio::buffer(buffer), error);
  while (!error) {
    writeLines(lines_.feed(std::span<const char>{buffer}.first(count)));
    count = pipe_.read_some(boost::asio::buffer(buffer), error);
  }
  if (const std::optional<std::string> last = lines_.finish();
      last.has_value()) {
    logging::writeStandardErrorLine(*last);
  }
}

}  // namespace rtype::server
