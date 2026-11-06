#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
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

/** @brief The cells an empty star lights, a cross, and a large cross. */
constexpr std::size_t EMPTY_STAR_PIXELS = 9;
constexpr std::size_t CROSS_PIXELS = 21;
constexpr std::size_t LARGE_CROSS_PIXELS = 33;

/** @brief A star near the top edge of the map (y 52), closer than the margin
 * of 3 pixels. */
constexpr sf::Vector2i STAR_NEAR_THE_TOP{200, 54};

/** @brief The name of a star of the tests. */
constexpr std::string_view STAR_NAME = "ORBITAL-EU1";

}  // namespace rtype::client::testing
