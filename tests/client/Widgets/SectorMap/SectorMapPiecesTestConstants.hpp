#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief A star in the middle of the map, a name 40 pixels wide, a star 3
 * pixels below it, and a star against the right edge of the map (x 450). */
constexpr sf::Vector2i MIDDLE_STAR{200, 130};
constexpr int NAME_WIDTH = 40;
constexpr sf::Vector2i STAR_JUST_BELOW{200, 133};
constexpr sf::Vector2i STAR_AT_THE_RIGHT_EDGE{450, 130};

/** @brief A forbidden box over the right of MIDDLE_STAR and another over its
 * left: with both, its name has no side. */
constexpr sf::IntRect OVER_THE_RIGHT{{206, 125}, {50, 9}};
constexpr sf::IntRect OVER_THE_LEFT{{140, 125}, {60, 9}};

/** @brief Stars for the arrows: one right of MIDDLE_STAR, one farther right,
 * one up and to the right at 60 degrees from the axis. */
constexpr sf::Vector2i NEAR_ON_THE_RIGHT{230, 132};

/** @brief Pointers by MIDDLE_STAR: 6 pixels right and 6 down, the farthest a
 * star is still picked, and 7 pixels right, just too far. */
constexpr sf::Vector2i AT_THE_EDGE_OF_THE_PICK{206, 136};
constexpr sf::Vector2i JUST_PAST_THE_PICK{207, 130};
constexpr sf::Vector2i FAR_ON_THE_RIGHT{300, 130};
constexpr sf::Vector2i STEEPLY_UP_RIGHT{210, 112};
constexpr std::size_t NO_SUCH_STAR = 9;

/** @brief Pilots of a server: none, a few, and more than ten. */
constexpr std::uint16_t NO_PILOT = 0;
constexpr std::uint16_t FEW_PILOTS = 3;
constexpr std::uint16_t CROWD_OF_PILOTS = 14;

/** @brief The sheet of the map, and the rectangle of the first frame of each
 * planet in it, from the top, rows 2 pixels apart: a small planet of 5 pixels
 * at top 0, a medium one of 7 at top 7, a large one of 9 at top 16, the slow
 * one of 9 at top 27, the one of another version of 7 at top 38; and the
 * house of the hangar, 9 pixels at top 47. */
constexpr std::string_view MAP_SHEET = "sprites/sector_map.png";
constexpr sf::IntRect SMALL_PLANET_SOURCE{{0, 0}, {5, 5}};
constexpr sf::IntRect MEDIUM_PLANET_SOURCE{{0, 7}, {7, 7}};
constexpr sf::IntRect LARGE_PLANET_SOURCE{{0, 16}, {9, 9}};
constexpr sf::IntRect SLOW_PLANET_SOURCE{{0, 27}, {9, 9}};
constexpr sf::IntRect OTHER_VERSION_PLANET_SOURCE{{0, 38}, {7, 7}};

/** @brief The fourth frame of the medium planet, three frames of 7 pixels
 * and 2 between them to the right: at left 27. */
constexpr std::size_t FOURTH_TURN = 3;
constexpr sf::IntRect MEDIUM_PLANET_FOURTH_SOURCE{{27, 7}, {7, 7}};

/** @brief Where a medium planet centered on MIDDLE_STAR (200, 130) starts:
 * 3 pixels up and to the left, half of its 7 pixels. */
constexpr sf::Vector2i MEDIUM_PLANET_AT_THE_MIDDLE{197, 127};

/** @brief Times of the turn of the planets, 150 ms a frame and 8 frames a
 * turn: the second frame starts at 150 ms, the last ends 1 ns before 1200
 * ms, and a whole turn is 1200 ms. */
constexpr std::chrono::milliseconds ONE_TURN_FRAME{150};
constexpr std::chrono::nanoseconds JUST_BEFORE_A_WHOLE_TURN{1'199'999'999};
constexpr std::chrono::milliseconds A_WHOLE_TURN{1200};
constexpr std::size_t LAST_TURN = 7;

/** @brief A star near the top edge of the map (y 52), closer than the margin
 * of 3 pixels. */
constexpr sf::Vector2i STAR_NEAR_THE_TOP{200, 54};

/** @brief The name of a star of the tests. */
constexpr std::string_view STAR_NAME = "ORBITAL-EU1";

}  // namespace rtype::client::testing
