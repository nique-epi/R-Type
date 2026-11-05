#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "CompressionConstants.hpp"

namespace rtype::network::testing {

/**
 * @brief One byte more than a datagram body holds.
 */
constexpr std::size_t ONE_BYTE_OVER_THE_LARGEST_BODY = MAXIMUM_BODY_SIZE + 1;

/**
 * @brief How many random byte strings are decoded, and the longest of them.
 */
constexpr std::size_t RANDOM_STRING_COUNT = 2000;
constexpr std::size_t LONGEST_RANDOM_STRING = 40;

/**
 * @brief The size a decoder is allowed to produce in the test that gives it
 * random strings, so a result over it is a bug.
 */
constexpr std::size_t SMALL_DECODE_LIMIT = 8;

/**
 * @brief The four bytes every zstd frame starts with (RFC 8878, section
 * 3.1.1), and how many random bytes follow them in a frame made of garbage.
 */
constexpr std::array<std::uint8_t, 4> FRAME_MAGIC_NUMBER{0x28, 0xB5, 0x2F,
                                                         0xFD};
constexpr std::size_t GARBAGE_AFTER_THE_MAGIC_SIZE = 20;

/**
 * @brief The frame header descriptor sits after the magic number (RFC 8878,
 * section 3.1.1). Its bits 0 and 1 hold the size of the dictionary identifier,
 * bit 2 the checksum flag, bit 5 the single segment flag and bits 6 and 7 the
 * size of the content field: all of them are 0 in the frames of the protocol,
 * which name no dictionary, carry no checksum and no size.
 */
constexpr std::size_t FRAME_DESCRIPTOR_OFFSET = 4;
constexpr std::uint8_t FRAME_OPTION_BITS = 0xE7;

}  // namespace rtype::network::testing
