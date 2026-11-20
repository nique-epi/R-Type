#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The rooms of the tests: two and three maps, and the votes for each
 * map, from 1; a vote for a third map, past the last of two. */
constexpr std::size_t TWO_TALLIED_MAPS = 2;
constexpr std::size_t THREE_TALLIED_MAPS = 3;
constexpr std::uint8_t VOTE_FOR_THE_FIRST = 1;
constexpr std::uint8_t VOTE_FOR_THE_SECOND = 2;
constexpr std::uint8_t VOTE_FOR_THE_THIRD = 3;

/** @brief The indices, from 0, votedMapOf() answers with. */
constexpr std::size_t FIRST_MAP_INDEX = 0;
constexpr std::size_t SECOND_MAP_INDEX = 1;
constexpr std::size_t THIRD_MAP_INDEX = 2;

/** @brief How many seeds a test of a draw tries: enough that every map of a
 * draw comes out, whatever the standard library draws. */
constexpr std::uint32_t SEEDS_TRIED = 200;

}  // namespace rtype::server::testing
