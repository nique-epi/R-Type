#pragma once

#include <cstddef>
#include <cstdint>

namespace rtype::client {

/** @returns What a slot of ShipLook holds when it wears @p item, the
 * position of an item in SHIP_LOOK_ITEM_NAMES: its position plus one, so
 * that NO_SHIP_LOOK_ITEM stays free. */
[[nodiscard]] constexpr std::uint8_t lookNumberOf(std::size_t item) {
  return static_cast<std::uint8_t>(item + 1);
}

/** @returns The position in SHIP_LOOK_ITEM_NAMES of the item a slot holding
 * @p number wears; @p number is never NO_SHIP_LOOK_ITEM. */
[[nodiscard]] constexpr std::size_t lookIndexOf(std::uint8_t number) {
  return static_cast<std::size_t>(number) - 1U;
}

}  // namespace rtype::client
