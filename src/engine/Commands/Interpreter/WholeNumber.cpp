#include "WholeNumber.hpp"
#include <charconv>
#include <cstdint>
#include <string>
#include <system_error>
#include "EngineException.hpp"

namespace rtype::engine {

std::int64_t wholeNumberFrom(const std::string& text) {
  std::int64_t number = 0;
  const char* const end = text.data() + text.size();
  const auto [stop, error] = std::from_chars(text.data(), end, number);
  if (text.empty() || error != std::errc{} || stop != end) {
    throw CommandRefusedException("value '" + text + "' is not a whole number");
  }
  return number;
}

}  // namespace rtype::engine
