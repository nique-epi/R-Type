#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "ItemListColumn.hpp"
#include "ItemListLayout.hpp"
#include "TextStyle.hpp"

namespace rtype::client::testing {

constexpr std::array LIST_COLUMNS{
    ItemListColumn{.anchor = 20, .alignment = TextAlignment::Right},
    ItemListColumn{.anchor = 30}};

/** @brief A list at (100, 50), 200 wide, a row every 13 pixels, two at most. */
constexpr ItemListLayout LIST_LAYOUT{.topLeft = {100, 50},
                                     .width = 200,
                                     .rowPitch = 13,
                                     .rowCapacity = 2,
                                     .columns = LIST_COLUMNS};

constexpr sf::Color CELL_COLOR(1, 2, 3);
constexpr sf::Color HIGHLIGHT_COLOR(4, 5, 6);

constexpr sf::Vector2i FIRST_ROW_FIRST_CELL(120, 50);
constexpr sf::Vector2i FIRST_ROW_SECOND_CELL(130, 50);
constexpr sf::Vector2i SECOND_ROW_SECOND_CELL(130, 63);

/** @brief Past each side of the row by 4, above its text by 2, 12 tall. */
constexpr sf::IntRect FIRST_ROW_HIGHLIGHT({96, 48}, {208, 12});

/** @brief With one row, the next row would start at 63: the line lies 2 above,
 * the pinned row 2 under. */
constexpr int PIN_LINE_ROW = 61;
constexpr sf::Vector2i PINNED_ROW_SECOND_CELL(130, 65);

/** @brief The tag of ACE, 24 wide in the regular font, 3 after it and 1 above
 * the row; its word, 18 wide in the small font of the recording canvas, 3 in.
 */
constexpr sf::IntRect ACE_TAG({157, 49}, {23, 9});
constexpr sf::Vector2i ACE_TAG_WORD(160, 49);

}  // namespace rtype::client::testing
