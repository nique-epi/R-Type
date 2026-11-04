#pragma once

#include <cstddef>
#include <string_view>

/** @brief The size of a key the master issues: 256 bits in hexadecimal. */
constexpr std::size_t EXPECTED_KEY_TEXT_SIZE = 64;

/** @brief Texts shaped almost like a key, which must all be refused. */
constexpr std::string_view KEY_OF_63_DIGITS =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde";
constexpr std::string_view KEY_OF_65_DIGITS =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef0";
constexpr std::string_view UPPERCASE_KEY =
    "0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF";
constexpr std::string_view KEY_WITH_A_G =
    "0123456789abcdeg0123456789abcdef0123456789abcdef0123456789abcdef";

/** @brief A text shaped like a key, which the master never issued. */
constexpr std::string_view KEY_NEVER_ISSUED =
    "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";
