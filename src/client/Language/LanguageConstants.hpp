#pragma once

#include <array>
#include <cstddef>
#include "Language.hpp"

namespace rtype::client {

/** @brief Every language, in the order the options list them. */
constexpr std::array LANGUAGES{Language::English, Language::French};
constexpr std::size_t LANGUAGE_COUNT = LANGUAGES.size();

/** @brief The mask and the value of a byte of UTF-8 that continues a letter:
 * 10xxxxxx. */
constexpr unsigned char UTF8_CONTINUATION_MASK = 0xC0U;
constexpr unsigned char UTF8_CONTINUATION_BITS = 0x80U;

}  // namespace rtype::client
