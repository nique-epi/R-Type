#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/**
 * @brief The most inputs before the newest that one PlayerInput repeats, as the
 * protocol page gives it.
 */
constexpr std::size_t REPEATED_INPUT_LIMIT = 15;

/** @brief Inputs sent in a row, more than a message repeats. */
constexpr std::uint32_t TWENTY_INPUTS = 20;

/** @brief Input numbers a test confirms or looks for. */
constexpr std::uint32_t THIRD_INPUT = 3;
constexpr std::uint32_t FIFTH_INPUT = 5;
constexpr std::uint32_t NINETEENTH_INPUT = 19;
constexpr std::uint32_t FAR_BEYOND_THE_NEWEST_INPUT = 1000;

/** @brief Keeps the five bits of the actions of an input number, so each of the
 * first inputs holds actions of its own. */
constexpr std::uint8_t ACTION_BITS_MASK = 0x1F;

/** @brief Actions used by name. */
constexpr std::uint8_t UP_ACTION = 0x01;
constexpr std::uint8_t DOWN_ACTION = 0x02;
constexpr std::uint8_t LEFT_ACTION = 0x04;
constexpr std::uint8_t SHOOT_ACTION = 0x10;

}  // namespace rtype::client::testing
