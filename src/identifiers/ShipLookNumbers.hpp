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

/** @returns Whether @p item, a position in SHIP_LOOK_ITEM_NAMES, is a hull: a
 * whole ship flown in place of the standard one. */
[[nodiscard]] constexpr bool isHullItem(std::size_t item) {
  return SHIP_LOOK_ITEM_SLOTS.at(item) == ShipLookSlot::Hull;
}

/** @returns Whether @p number names a hull of the catalog. */
[[nodiscard]] constexpr bool isHullNumber(std::uint8_t number) {
  return isLookItemNumber(number) && isHullItem(lookIndexOf(number));
}

/** @returns Whether @p number names an item drawn over the ship: an item of
 * the catalog that is no hull. Only such an item floats in a game. */
[[nodiscard]] constexpr bool isAccessoryNumber(std::uint8_t number) {
  return isLookItemNumber(number) && !isHullItem(lookIndexOf(number));
}

/** @returns The place of the hull @p number among the hulls, 0 for the first:
 * its block of the hull sheet. @p number is a hull (see isHullNumber()). */
[[nodiscard]] constexpr std::size_t hullIndexOf(std::uint8_t number) {
  return lookIndexOf(number) - SHIP_LOOK_ACCESSORY_COUNT;
}

/** @returns Whether the catalog lists its SHIP_LOOK_ACCESSORY_COUNT
 * accessories first and its hulls after them, which hullIndexOf() counts on. */
[[nodiscard]] constexpr bool accessoriesComeFirst() {
  for (std::size_t item = 0; item < SHIP_LOOK_ITEM_COUNT; ++item) {
    if (isHullItem(item) != (item >= SHIP_LOOK_ACCESSORY_COUNT)) {
      return false;
    }
  }
  return true;
}

static_assert(accessoriesComeFirst(),
              "the hulls close the catalog, after every accessory");

}  // namespace rtype::identifiers
