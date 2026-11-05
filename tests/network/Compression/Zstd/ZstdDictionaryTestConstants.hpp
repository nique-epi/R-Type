#pragma once

#include <array>
#include <cstdint>

namespace rtype::network::testing {

/**
 * @brief The four bytes every zstd dictionary starts with, 0xEC30A437 written
 * little-endian (zstd's zdict.h, ZSTD_MAGIC_DICTIONARY).
 */
constexpr std::array<std::uint8_t, 4> DICTIONARY_MAGIC_NUMBER{0x37, 0xA4, 0x30,
                                                              0xEC};

}  // namespace rtype::network::testing
