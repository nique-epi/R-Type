#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

/** @brief What the journal holds when a program opens a simulated network,
 * and how many times it says so. */
constexpr std::string_view SIMULATED_NETWORK_ANNOUNCEMENT = "network simulated";
constexpr std::size_t ONE_ANNOUNCEMENT = 1;

/** @brief The port of the peer of the tests, on the loopback address. */
constexpr std::uint16_t PEER_PORT = 4243;

/** @brief The bytes the tests hand over, each one different from the next,
 * so a lost, shifted or swapped byte shows. */
constexpr std::array<std::byte, 3> CROSSING_BYTES{
    std::byte{0x0A}, std::byte{0x0B}, std::byte{0x0C}};
