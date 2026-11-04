#pragma once

#include <cstdint>

namespace rtype::server::testing {

/** @brief The datagrams lost at random: 10 percent, from a fixed seed. */
constexpr std::uint32_t LOSS_SEED = 20261104;
constexpr double TEN_PERCENT_LOSS = 0.10;

/** @brief Inputs the player sends in the test: 2000, 33 seconds at 60 per
 * second. */
constexpr std::uint32_t INPUTS_SENT = 2000;

/**
 * @brief The server confirms what it applied in a WorldState every third input
 * (20 per second against 60 inputs), and the confirmation takes 9 inputs
 * (150 ms) to come back.
 */
constexpr std::uint32_t INPUTS_BETWEEN_CONFIRMATIONS = 3;
constexpr std::uint32_t INPUTS_A_CONFIRMATION_TAKES = 9;

/** @brief Keeps the five bits of the actions of an input number, so the actions
 * of neighbouring inputs differ. */
constexpr std::uint8_t ACTION_BITS_MASK = 0x1F;

}  // namespace rtype::server::testing
