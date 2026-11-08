#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief The panel of a dialog lies at (104, 87): (480 - 272) / 2 and
 * (270 - 96) / 2. The label of a row starts 8 in and 4 down from a row 8 in
 * from the panel; rows stand 28 down, 18 apart. */
constexpr sf::Vector2i FIRST_ROW_LABEL(120, 119);
constexpr sf::Vector2i SECOND_ROW_LABEL(120, 137);
/** @brief BACK of a dialog of two rows: its bar 66 down the panel, at x 160,
 * the label 12 in and 6 down. */
constexpr sf::Vector2i BACK_UNDER_TWO_ROWS(172, 159);

/** @brief A dialog of three rows, as the picture of the new game dialog draws
 * it: a panel of 272 x 114 at (104, 78), (270 - 114) / 2 = 78. Its heading
 * stands 16 in and 12 down, its third row 28 + 2 x 18 down, its BACK bar 84
 * down. */
constexpr sf::Vector2i HEADING_OVER_THREE_ROWS(120, 90);
constexpr sf::Vector2i THIRD_ROW_LABEL(120, 146);
constexpr sf::Vector2i BACK_UNDER_THREE_ROWS(172, 168);

}  // namespace rtype::client::testing
