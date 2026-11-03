#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief The panel of a dialog lies at (104, 87): (480 - 272) / 2 and
 * (270 - 96) / 2. The label of a row starts 8 in and 4 down from a row 8 in
 * from the panel; rows stand 28 down, 18 apart. */
constexpr sf::Vector2i FIRST_ROW_LABEL(120, 119);
constexpr sf::Vector2i SECOND_ROW_LABEL(120, 137);

}  // namespace rtype::client::testing
