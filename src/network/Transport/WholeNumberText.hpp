#pragma once

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>

namespace rtype::network {

/**
 * @returns The whole number @p text writes in decimal digits only (no sign, no
 * space, nothing after the digits), when it fits in a Number; nothing for any
 * other text, an empty one included.
 */
template <class Number>
[[nodiscard]] std::optional<Number> wholeNumberIn(std::string_view text) {
  Number number{};
  const char* const begin = text.data();
  const char* const end = begin + text.size();
  const auto [parsedEnd, error] = std::from_chars(begin, end, number);
  if (error != std::errc{} || parsedEnd != end) {
    return std::nullopt;
  }
  return number;
}

}  // namespace rtype::network
