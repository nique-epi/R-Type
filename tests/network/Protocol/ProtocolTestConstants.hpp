#pragma once

#include <array>
#include <string_view>

namespace rtype::network::testing {

/**
 * @brief A coordinate of minus one logical unit, which the wire carries as -8
 * eighths.
 */
constexpr float MINUS_ONE_UNIT = -1.0F;

/**
 * @brief Nicknames of 1 and 16 bytes, the ends of the sizes the protocol
 * accepts, and one of 17 bytes.
 */
constexpr std::string_view ONE_BYTE_NICKNAME = "A";
constexpr std::string_view SIXTEEN_BYTE_NICKNAME = "abcdefghijklmnop";
constexpr std::string_view SEVENTEEN_BYTE_NICKNAME = "abcdefghijklmnopq";

/**
 * @brief Four rockets, U+1F680, of four bytes each: sixteen bytes.
 */
constexpr std::string_view FOUR_ROCKETS =
    "\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80\xF0\x9F\x9A\x80";

/**
 * @brief The letter e with an acute accent, U+00E9, in two bytes.
 */
constexpr std::string_view ACCENTED_LETTER = "\xC3\xA9";

/**
 * @brief Well-formed UTF-8 at the edges of the table of RFC 3629: the last
 * one-byte character, the first and last two-byte ones, the first three-byte
 * one, the last before the surrogates, the first after them, the first
 * four-byte one and U+10FFFF, the last code point.
 */
constexpr auto WELL_FORMED_UTF8_AT_THE_EDGES = std::to_array<std::string_view>({
    "\x7F",
    "\xC2\x80",
    "\xDF\xBF",
    "\xE0\xA0\x80",
    "\xED\x9F\xBF",
    "\xEE\x80\x80",
    "\xF0\x90\x80\x80",
    "\xF4\x8F\xBF\xBF",
});

/**
 * @brief Ill-formed UTF-8: an overlong two-byte and three-byte form, a
 * surrogate, a code point above U+10FFFF, a byte no sequence starts with, a
 * stray continuation byte and a sequence cut short.
 */
constexpr auto ILL_FORMED_UTF8 = std::to_array<std::string_view>({
    "\xC0\x80",
    "\xE0\x80\x80",
    "\xED\xA0\x80",
    "\xF4\x90\x80\x80",
    "\xF5\x80\x80\x80",
    "\x80",
    "\xE2\x82",
});

}  // namespace rtype::network::testing
