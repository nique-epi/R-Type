#pragma once

#include <cstddef>
#include <string_view>

/**
 * @brief The shape of a key the master issues: its prefix, then 256 bits in
 * hexadecimal, 4 + 64 characters.
 */
constexpr std::string_view EXPECTED_KEY_PREFIX = "rtk_";
constexpr std::size_t EXPECTED_KEY_SIZE = 68;
