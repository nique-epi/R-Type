#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/**
 * @brief The sequence a ledger accepted, the one half the range away from it,
 * and the one just before that distance.
 */
constexpr std::uint16_t HALF_RANGE_DISTANCE = 32768;
constexpr std::uint16_t JUST_UNDER_HALF_RANGE_DISTANCE =
    HALF_RANGE_DISTANCE - 1;

/**
 * @brief A loss of one datagram in ten: every LOSS_PERIOD-th datagram of a
 * window is skipped, the first one skipped being at LOST_OFFSET.
 */
constexpr std::size_t LOSS_PERIOD = 10;
constexpr std::size_t LOST_OFFSET = 5;
constexpr double ONE_IN_TEN = 0.1;

}  // namespace rtype::server::testing
