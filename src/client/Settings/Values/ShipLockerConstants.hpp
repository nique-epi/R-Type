#pragma once

#include <cstddef>
#include <string_view>
#include "ShipLookConstants.hpp"

namespace rtype::client {

/** @brief The items of a first launch: the top hat, the cool shades and the
 * mustache, one per slot, as bits of the locker (bit n is the item at place
 * n of identifiers::SHIP_LOOK_ITEM_NAMES). */
constexpr std::size_t STARTER_TOP_HAT = 0;
constexpr std::size_t STARTER_COOL_SHADES = 8;
constexpr std::size_t STARTER_MUSTACHE = 9;
constexpr unsigned long long STARTER_LOCKER_ITEMS =
    (1ULL << STARTER_TOP_HAT) | (1ULL << STARTER_COOL_SHADES) |
    (1ULL << STARTER_MUSTACHE);

static_assert(identifiers::SHIP_LOOK_ITEM_NAMES.at(STARTER_TOP_HAT) ==
                  std::string_view{"top_hat"},
              "the first starter item is the top hat");
static_assert(identifiers::SHIP_LOOK_ITEM_NAMES.at(STARTER_COOL_SHADES) ==
                  std::string_view{"cool_shades"},
              "the second starter item is the cool shades");
static_assert(identifiers::SHIP_LOOK_ITEM_NAMES.at(STARTER_MUSTACHE) ==
                  std::string_view{"mustache"},
              "the third starter item is the mustache");

}  // namespace rtype::client
