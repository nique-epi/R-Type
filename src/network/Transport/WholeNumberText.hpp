#pragma once

#include <charconv>
#include <string_view>
#include <system_error>

namespace rtype::network {

/**
 * @brief Reads @p text as a whole number written in decimal digits only: no
 * sign, no space, nothing after the digits.
 * @returns Whether @p text is such a number and it fits in a Number; @p number
 * then holds it.
 */
template <class Number>
[[nodiscard]] bool readsWholeNumber(std::string_view text, Number& number) {
  const char* const begin = text.data();
  const char* const end = begin + text.size();
  const auto [parsedEnd, error] = std::from_chars(begin, end, number);
  return error == std::errc{} && parsedEnd == end;
}

}  // namespace rtype::network
