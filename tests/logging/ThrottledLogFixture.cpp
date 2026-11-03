#include "ThrottledLogFixture.hpp"
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>
#include "ThrottledLogTestConstants.hpp"

namespace rtype::logging::testing {

void ThrottledLogFixture::eventAt(std::chrono::nanoseconds now) {
  log_.debug(now, EVENT_TEXT);
}

void ThrottledLogFixture::errorAt(std::chrono::nanoseconds now) {
  log_.error(now, EVENT_TEXT);
}

std::vector<std::string> ThrottledLogFixture::lines() const {
  return journal_.linesHolding(EVENT_TEXT);
}

std::string ThrottledLogFixture::countEnding(std::size_t count) {
  return std::string{COUNT_ENDING_OPENING} + std::to_string(count) +
         std::string{COUNT_ENDING_CLOSING};
}

}  // namespace rtype::logging::testing
