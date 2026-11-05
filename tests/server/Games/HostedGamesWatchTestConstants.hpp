#pragma once

#include <cstdint>
#include <string_view>

namespace rtype::server::testing {

/** @brief The difficulty the watched games play, and a player of theirs. */
constexpr std::string_view WATCHED_DIFFICULTY = "normal";
constexpr std::uint8_t ARRIVING_PLAYER = 1;

/** @brief The revisions of the list: none, the first, then a second one. */
constexpr std::uint64_t NO_LIST = 0;
constexpr std::uint64_t FIRST_LIST = 1;
constexpr std::uint64_t SECOND_LIST = 2;

}  // namespace rtype::server::testing
