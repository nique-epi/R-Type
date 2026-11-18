#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::server::testing {

/** @brief The seats of the tests, from 0. */
constexpr std::size_t FIRST_SEAT = 0;
constexpr std::size_t SECOND_SEAT = 1;

/** @brief The number of the crown among the items of the locker: its place
 * in the catalog, 2, plus one. */
constexpr std::uint8_t CROWN_NUMBER = 3;

}  // namespace rtype::server::testing
