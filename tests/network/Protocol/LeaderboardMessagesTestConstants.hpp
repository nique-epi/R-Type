#pragma once

#include <cstddef>
#include <cstdint>
#include "ProtocolConstants.hpp"

namespace rtype::network::testing {

constexpr unsigned int TOO_MANY_ROWS = MAXIMUM_LEADERBOARD_ROWS + 1;
constexpr std::uint32_t SOME_RANK = 2;
constexpr std::uint32_t SOME_LEADERBOARD_SCORE = 500;
constexpr std::int64_t SOME_PLAYED_AT = 1;

/** @brief The largest payload of a LeaderboardAnswer, added up by hand from the
 * table of the protocol page: 1 + 1 + 16 + 1 + 10 x 33 + 1 + 33. */
constexpr std::size_t LARGEST_ANSWER_ON_THE_PROTOCOL_PAGE = 383;

}  // namespace rtype::network::testing
