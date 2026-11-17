#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/**
 * @brief The rail of the tests: ONE, open, TWO, and LOCK, locked, centred on
 * x 240 with its labels at row 30. Labels are 8 pixels a letter and 20 apart,
 * and the badge of LOCK adds 4 and 28: ONE from x 164 to 188, TWO from 208 to
 * 232, LOCK and its badge from 252 to 316; a tab answers from row 26 to 43.
 */
constexpr const char* FIRST_RAIL_LABEL = "ONE";
constexpr const char* SECOND_RAIL_LABEL = "TWO";
constexpr const char* LOCKED_RAIL_LABEL = "LOCK";
constexpr std::string_view LOCKED_BADGE = "SOON";
constexpr sf::Vector2i RAIL_TOP_MIDDLE{240, 30};
constexpr std::size_t FIRST_RAIL_TAB = 0;
constexpr std::size_t SECOND_RAIL_TAB = 1;
constexpr std::size_t LOCKED_RAIL_TAB = 2;
constexpr int FIRST_RAIL_TAB_LEFT = 164;
constexpr int FIRST_RAIL_TAB_RIGHT = 188;
constexpr sf::Vector2i IN_THE_SECOND_RAIL_TAB{220, 34};
constexpr sf::Vector2i BETWEEN_THE_FIRST_TWO_RAIL_TABS{198, 34};
constexpr sf::Vector2i UNDER_THE_RAIL{220, 50};

/** @brief The colors of the design system: star white for the open tab and
 * the tab under the pointer, pale blue for the others, blue for a locked one,
 * cyan for the bar. */
constexpr sf::Color OPEN_LABEL_COLOR{238, 241, 255};
constexpr sf::Color IDLE_LABEL_COLOR{143, 162, 230};
constexpr sf::Color LOCKED_LABEL_COLOR{74, 90, 168};
constexpr sf::Color INDICATOR_COLOR{127, 230, 255};

/** @brief A time inside the shake of a refused tab, and one past its end
 * (280 ms). */
constexpr std::chrono::milliseconds INSIDE_THE_SHAKE{10};
constexpr std::chrono::milliseconds PAST_THE_SHAKE{300};

}  // namespace rtype::client::testing
