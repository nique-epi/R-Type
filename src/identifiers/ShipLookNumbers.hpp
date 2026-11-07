#pragma once

#include <cstddef>
#include <cstdint>
#include "ShipLookConstants.hpp"
#include "ShipLookSlot.hpp"

namespace rtype::identifiers {

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

/** @returns Whether @p number names an item of the catalog: 1 to
 * SHIP_LOOK_ITEM_COUNT. */
[[nodiscard]] constexpr bool isLookItemNumber(std::uint8_t number) {
  return number != NO_SHIP_LOOK_ITEM && number <= SHIP_LOOK_ITEM_COUNT;
}

/** @returns Whether a slot @p slot may hold @p number: NO_SHIP_LOOK_ITEM, or
 * the number of an item of that slot. */
[[nodiscard]] constexpr bool isLookNumberOf(std::uint8_t number,
                                            ShipLookSlot slot) {
  return number == NO_SHIP_LOOK_ITEM ||
         (isLookItemNumber(number) &&
          SHIP_LOOK_ITEM_SLOTS.at(lookIndexOf(number)) == slot);
}

}  // namespace rtype::identifiers
