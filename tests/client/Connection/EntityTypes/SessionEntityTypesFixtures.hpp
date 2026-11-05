#pragma once

#include <string_view>
#include <vector>
#include "EntityTypeEntry.hpp"

namespace rtype::client::testing {

/** @returns Whether the client of the tests draws the type @p name:
 * DRAWN_ENEMY_NAME, DRAWN_SHIP_NAME and OTHER_DRAWN_ENEMY_NAME only. */
[[nodiscard]] bool isDrawnInTheTests(std::string_view name);

/** @returns A table the tests draw: DRAWN_ENEMY_NAME, an enemy, then
 * DRAWN_SHIP_NAME, a ship. */
[[nodiscard]] std::vector<network::EntityTypeEntry> drawnTypes();

/** @returns Another table the tests draw: OTHER_DRAWN_ENEMY_NAME alone. */
[[nodiscard]] std::vector<network::EntityTypeEntry> otherDrawnTypes();

/** @returns A table with one type the tests cannot draw: DRAWN_ENEMY_NAME,
 * then UNDRAWN_ENEMY_NAME. */
[[nodiscard]] std::vector<network::EntityTypeEntry> oneUndrawnType();

/** @returns A table with three types the tests cannot draw, among drawn ones:
 * UNDRAWN_ENEMY_NAME, DRAWN_SHIP_NAME, OTHER_UNDRAWN_ENEMY_NAME,
 * THIRD_UNDRAWN_ENEMY_NAME. */
[[nodiscard]] std::vector<network::EntityTypeEntry> threeUndrawnTypes();

}  // namespace rtype::client::testing
