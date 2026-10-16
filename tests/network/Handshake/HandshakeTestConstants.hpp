#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include "HandshakeConstants.hpp"
#include "SerializationConstants.hpp"

namespace rtype::network::testing {

/**
 * @brief Outputs of SipHash-2-4 with a 64-bit output, copied from vectors.h of
 * the reference implementation (github.com/veorq/SipHash), in the order it
 * writes them: entry n is the hash of the n bytes 0, 1, ..., n - 1 under the
 * key 0, 1, ..., 15.
 */
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_0_BYTES{
    0x31, 0x0e, 0x0e, 0xdd, 0x47, 0xdb, 0x6f, 0x72};
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_1_BYTE{
    0xfd, 0x67, 0xdc, 0x93, 0xc5, 0x39, 0xf8, 0x74};
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_2_BYTES{
    0x5a, 0x4f, 0xa9, 0xd9, 0x09, 0x80, 0x6c, 0x0d};
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_3_BYTES{
    0x2d, 0x7e, 0xfb, 0xd7, 0x96, 0x66, 0x67, 0x85};
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_8_BYTES{
    0x62, 0x24, 0x93, 0x9a, 0x79, 0xf5, 0xf5, 0x93};
constexpr std::array<std::uint8_t, UINT64_WIRE_SIZE> REFERENCE_HASH_OF_15_BYTES{
    0xe5, 0x45, 0xbe, 0x49, 0x61, 0xca, 0x29, 0xa1};

/**
 * @brief Message lengths of the reference outputs above.
 */
constexpr std::size_t ONE_BYTE = 1;
constexpr std::size_t TWO_BYTES = 2;
constexpr std::size_t THREE_BYTES = 3;
constexpr std::size_t ONE_BLOCK = UINT64_WIRE_SIZE;
constexpr std::size_t ONE_BLOCK_AND_SEVEN_BYTES = 15;

/**
 * @brief A sender of connection requests, and others that differ from it by
 * the address or by the port only.
 */
constexpr std::string_view SENDER_ADDRESS = "192.0.2.10";
constexpr std::string_view OTHER_SENDER_ADDRESS = "192.0.2.11";
constexpr std::uint16_t SENDER_PORT = 50000;
constexpr std::uint16_t OTHER_SENDER_PORT = 50001;

/**
 * @brief Moments inside cookie windows: the start of a window, its last
 * nanosecond, the start of the next one, and the start of the one after.
 */
constexpr std::chrono::nanoseconds START_OF_A_WINDOW =
    COOKIE_WINDOW_DURATION * 3;
constexpr std::chrono::nanoseconds END_OF_THAT_WINDOW =
    START_OF_A_WINDOW + COOKIE_WINDOW_DURATION - std::chrono::nanoseconds{1};
constexpr std::chrono::nanoseconds START_OF_THE_NEXT_WINDOW =
    START_OF_A_WINDOW + COOKIE_WINDOW_DURATION;
constexpr std::chrono::nanoseconds END_OF_THE_NEXT_WINDOW =
    START_OF_THE_NEXT_WINDOW + COOKIE_WINDOW_DURATION -
    std::chrono::nanoseconds{1};
constexpr std::chrono::nanoseconds TWO_WINDOWS_LATER =
    START_OF_A_WINDOW + (2 * COOKIE_WINDOW_DURATION);

/**
 * @brief A byte that differs between the two keys of a test.
 */
constexpr std::byte OTHER_KEY_BYTE{0xff};

}  // namespace rtype::network::testing
