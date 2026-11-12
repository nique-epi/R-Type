#pragma once

#include <cstddef>

namespace rtype::client::testing {

/** @brief What the shop sells: the 25 accessories but the three of the
 * starter kit, and the five hulls; the top hat, the first item of the
 * catalog, is one of the starter kit. */
constexpr std::size_t LOOKS_FOR_SALE = 22;
constexpr std::size_t HULLS_FOR_SALE = 5;
constexpr std::size_t TOP_HAT_ITEM = 0;

}  // namespace rtype::client::testing
