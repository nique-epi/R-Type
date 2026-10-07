#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace rtype::network {

constexpr std::size_t BITS_PER_BYTE = 8;
constexpr std::size_t UINT8_WIRE_SIZE = sizeof(std::uint8_t);
constexpr std::size_t UINT16_WIRE_SIZE = sizeof(std::uint16_t);
constexpr std::size_t UINT32_WIRE_SIZE = sizeof(std::uint32_t);
constexpr std::size_t UINT64_WIRE_SIZE = sizeof(std::uint64_t);

constexpr std::size_t SHORT_STRING_PREFIX_SIZE = UINT8_WIRE_SIZE;
constexpr std::size_t MAX_SHORT_STRING_LENGTH =
    std::numeric_limits<std::uint8_t>::max();

constexpr std::uint64_t BYTE_MASK = 0xFF;

}  // namespace rtype::network
