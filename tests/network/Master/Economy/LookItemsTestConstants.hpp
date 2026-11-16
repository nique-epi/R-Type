#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace rtype::network::testing {

/** @brief The crown, the third item of identifiers::SHIP_LOOK_ITEM_NAMES, and
 * names no item has: the crown in capitals, and an empty name. */
constexpr std::string_view CROWN_ITEM_NAME = "crown";
constexpr std::size_t CROWN_ITEM = 2;
constexpr std::string_view CROWN_IN_CAPITALS = "CROWN";
constexpr std::string_view NO_ITEM_NAME = "";

/** @brief Looks in no order, a name of no look among them, and the same in
 * the order of the catalogue: top hat, crown, mustache, then the unknown. */
inline const std::vector<std::string> LOOKS_IN_NO_ORDER{
    "mustache", "no_such_look", "crown", "top_hat"};
inline const std::vector<std::string> LOOKS_IN_CATALOGUE_ORDER{
    "top_hat", "crown", "mustache", "no_such_look"};

}  // namespace rtype::network::testing
