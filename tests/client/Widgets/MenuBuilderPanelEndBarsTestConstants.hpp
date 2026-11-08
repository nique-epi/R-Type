#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client::testing {

/** @brief The panel of rows of a tabbed panel spans x 24 to 456: the safe
 * margin of 24, then 480 - 2 x 24 = 432. Its bars stand at y 224, 80 wide,
 * their label 12 in and 6 down. A bar asked for after the cancel button ends
 * at the right end of the panel: it starts at 456 - 80 = 376, its label at
 * 388. */
constexpr sf::Vector2i LABEL_OF_THE_BAR_AT_THE_PANEL_END(388, 230);

/** @brief The description stays 8 right of BACK, which spans x 24 to 104, and
 * not right of the bar at the end of the panel, which would put it at 464. */
constexpr sf::Vector2i DESCRIPTION_RIGHT_OF_BACK(112, 230);

}  // namespace rtype::client::testing
