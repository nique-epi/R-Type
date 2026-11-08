#pragma once

#include <cstdint>

namespace rtype::client::testing {

/** @brief A datagram right after the newest: nothing skipped. */
constexpr std::uint16_t NEXT_DATAGRAM_DISTANCE = 1;

/** @brief A datagram three sequences after the newest: two skipped. */
constexpr std::uint16_t THREE_SEQUENCES_AHEAD = 3;
constexpr std::uint64_t TWO_SKIPPED = 2;

/** @brief The farthest a newer datagram can be, 32767 (see
 * isNewerSequence): 32766 skipped. */
constexpr std::uint16_t FARTHEST_NEWER_DISTANCE = 32767;
constexpr std::uint64_t MOST_SKIPPED = 32766;

}  // namespace rtype::client::testing
