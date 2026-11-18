#pragma once

#include <cstddef>
#include <string_view>

namespace rtype::network::testing {

/** @brief The crown, the third item of identifiers::SHIP_LOOK_ITEM_NAMES, and
 * names no item has: the crown in capitals, and an empty name. */
constexpr std::string_view CROWN_ITEM_NAME = "crown";
constexpr std::size_t CROWN_ITEM = 2;
constexpr std::string_view CROWN_IN_CAPITALS = "CROWN";
constexpr std::string_view NO_ITEM_NAME{};

}  // namespace rtype::network::testing
