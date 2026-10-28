#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief The sequence of the datagram a message came in, and the sequences
 * just before and after it.
 */
constexpr std::uint16_t SOME_SEQUENCE = 40;
constexpr std::uint16_t OLDER_SEQUENCE = SOME_SEQUENCE - 1;
constexpr std::uint16_t NEWER_SEQUENCE = SOME_SEQUENCE + 1;

/** @brief The last sequence of a 16-bit counter, and the one that follows it.
 */
constexpr std::uint16_t LAST_SEQUENCE = 65535;
constexpr std::uint16_t FIRST_SEQUENCE = 0;

/**
 * @brief Numbers of the inputs a player sent, in the order it numbers them.
 */
constexpr std::uint32_t SOME_INPUT_NUMBER = 100;
constexpr std::uint32_t OLDER_INPUT_NUMBER = SOME_INPUT_NUMBER - 1;
constexpr std::uint32_t NEWER_INPUT_NUMBER = SOME_INPUT_NUMBER + 1;

/**
 * @brief Two sets of actions that differ.
 */
constexpr std::uint8_t SOME_ACTIONS = 0x01;
constexpr std::uint8_t OTHER_ACTIONS = 0x04;

/**
 * @brief How many players the keepers are given.
 */
constexpr std::size_t TWO_PLAYERS = 2;

}  // namespace rtype::server::testing
