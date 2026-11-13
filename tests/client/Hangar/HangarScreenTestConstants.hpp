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
    "LOCKED: WIN IT IN A MISSION OR BUY IT";
constexpr int CROWN_ICON_LEFT_ON_THE_SHEET = 36;
constexpr const char* LOOK_SHEET_ID = "sprites/looks.png";
constexpr std::string_view PARTY_HAT_STATUS = "PARTY HAT - ENTER: WEAR";

/** @brief Four presses of Down from the first item leave the fourth row:
 * the locker scrolls one row. The cell of the crown then holds item 5, the
 * viking helmet, number 6; after one notch of the wheel, the first cell,
 * middle (52, 70), holds item 3, the propeller cap, number 4. */
constexpr int DOWNS_PAST_THE_ROWS_SHOWN = 4;
constexpr std::uint8_t WEARS_VIKING_HELMET = 6;
constexpr sf::Vector2i FIRST_CELL_MIDDLE{52, 70};
constexpr std::uint8_t WEARS_PROPELLER_CAP = 4;
constexpr float ONE_NOTCH_DOWN = -1.0F;
/** @brief The thumb of the scroll mark: 2 x 63 at (158, 52) when the hangar
 * opens, 16 rows lower once the locker scrolled one row. */
constexpr sf::IntRect THUMB_AT_OPENING{{158, 52}, {2, 63}};
constexpr sf::IntRect THUMB_AFTER_ONE_ROW{{158, 68}, {2, 63}};
/** @brief The war shark, item 25: eight rows down and one cell right of the
 * first item; the ship then holds hull number 26. */
constexpr int DOWNS_TO_THE_ROW_OF_THE_HULLS = 8;
constexpr std::uint8_t WEARS_WAR_SHARK = 26;
constexpr std::string_view LOCKED_HULL_STATUS_TEXT =
    "LOCKED: BUY IT AT THE SHOP";

}  // namespace rtype::client::testing
