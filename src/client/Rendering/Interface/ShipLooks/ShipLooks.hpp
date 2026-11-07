#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <cstddef>
#include <vector>
#include "IInterfaceCanvas.hpp"
#include "ShipLook.hpp"
#include "ShipPlacement.hpp"

namespace rtype::client {

/** @returns @p look wearing @p item, the position of an item in
 * SHIP_LOOK_ITEM_NAMES, in place of what its slot wore. */
[[nodiscard]] ShipLook wearing(ShipLook look, std::size_t item);

/** @returns @p look without @p item; another item of its slot stays. */
[[nodiscard]] ShipLook without(ShipLook look, std::size_t item);

/** @returns Whether the slot of @p item wears it. */
[[nodiscard]] bool wears(ShipLook look, std::size_t item);

/** @returns The items @p look wears, in the order of SHIP_LOOK_ITEM_NAMES:
 * the order they are drawn in. */
[[nodiscard]] std::vector<std::size_t> wornItemsOf(ShipLook look);

/** @returns The rectangle of the drawing of @p item in SHIP_LOOK_SHEET_ID. */
[[nodiscard]] sf::IntRect lookRectangleOf(std::size_t item);

/** @brief Draws @p item where it sits on a ship drawn at @p ship. */
void paintLookItem(IInterfaceCanvas& canvas, std::size_t item,
                   ShipPlacement ship);

/** @brief Draws every item @p look wears on a ship drawn at @p ship. */
void paintShipLook(IInterfaceCanvas& canvas, ShipLook look, ShipPlacement ship);

/** @brief Draws @p item centred in @p cell, at the largest whole scale, up to
 * MAXIMUM_LOOK_ICON_SCALE, at which it fits. */
void paintLookIcon(IInterfaceCanvas& canvas, std::size_t item,
                   sf::IntRect cell);

}  // namespace rtype::client
