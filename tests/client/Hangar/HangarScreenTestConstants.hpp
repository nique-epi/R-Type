#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief What the head holds for the top hat and the crown: their place in
 * the locker plus one; nothing. */
constexpr std::uint8_t WEARS_TOP_HAT = 1;
constexpr std::uint8_t WEARS_CROWN = 3;
constexpr std::uint8_t WEARS_NOTHING = 0;
/** @brief The ship of the first player color, row 0 of the sheet. */
constexpr int FIRST_PILOT_COLOR = 0;
/** @brief The middle of the cell of the crown, the third of the first row:
 * x 24 + 10 + 2 x 41 + 18 = 134, y 34 + 18 + 18 = 70. */
constexpr sf::Vector2i CROWN_CELL_MIDDLE{134, 70};
/** @brief The top hat in the sheet: the first cell, 14 x 10 from its corner.
 */
constexpr sf::IntRect TOP_HAT_IN_THE_SHEET{{0, 0}, {14, 10}};
/** @brief The scale the ship of the hangar is drawn at: four times. */
constexpr int SHIP_SCALE_OF_THE_HANGAR = 4;
constexpr std::string_view LOOK_SHEET = "sprites/looks.png";
constexpr std::string_view LOCKED_STATUS_TEXT =
    "LOCKED: PICK IT UP IN A MISSION";
constexpr int CROWN_ICON_LEFT_ON_THE_SHEET = 36;
constexpr const char* LOOK_SHEET_ID = "sprites/looks.png";
constexpr std::string_view PARTY_HAT_STATUS = "PARTY HAT - ENTER: WEAR";

}  // namespace rtype::client::testing
