#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include "LanguageConstants.hpp"
#include "Translation.hpp"

namespace rtype::client {

/** @returns How many letters the UTF-8 @p text holds: its bytes that do not
 * continue a letter. Usable at compile time, so a constant can check that its
 * longest translation fits a line. */
[[nodiscard]] constexpr std::size_t lettersIn(std::string_view text) {
  std::size_t letters = 0;
  for (const char byte : text) {
    if ((static_cast<unsigned char>(byte) & UTF8_CONTINUATION_MASK) !=
        UTF8_CONTINUATION_BITS) {
      ++letters;
    }
  }
  return letters;
}

/** @returns The letters of the longest language of @p text. */
[[nodiscard]] constexpr std::size_t longestLettersIn(const Translation& text) {
  return std::max(lettersIn(text.english), lettersIn(text.french));
}

}  // namespace rtype::client
