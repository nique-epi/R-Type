#pragma once

#include <cstddef>

namespace rtype::client::testing {

/**
 * @brief The rate of the inputs the protocol page gives: 60 per second.
 */
constexpr std::size_t PROTOCOL_INPUTS_PER_SECOND = 60;

/**
 * @brief How many intervals between two inputs a stall of the frame loop lasts
 * in the tests.
 */
constexpr int INTERVALS_OF_A_STALL = 3;

}  // namespace rtype::client::testing
