#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

/** @brief The port of the peer of the tests, on the loopback address. */
constexpr std::uint16_t PEER_PORT = 4243;

/** @brief The bytes the tests hand over, each one different from the next,
 * so a lost, shifted or swapped byte shows. */
constexpr std::array<std::byte, 3> CROSSING_BYTES{
    std::byte{0x0A}, std::byte{0x0B}, std::byte{0x0C}};
