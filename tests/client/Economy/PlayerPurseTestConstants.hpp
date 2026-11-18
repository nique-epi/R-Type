#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/** @brief Settings holding 4 250 kopecks, 1 250 chips and the crown, the item
 * at place 2 of the locker. */
constexpr std::uint32_t KOPECKS_OF_THE_SETTINGS = 4'250;
constexpr std::uint32_t CHIPS_OF_THE_SETTINGS = 1'250;
constexpr std::size_t CROWN_IN_THE_LOCKER = 2;

}  // namespace rtype::client::testing
