#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string_view>

/** @brief The image of a near star: 6 by 3 pixels, two frames of 3 by 3
 * side by side. Its second frame is its right half. */
constexpr sf::Vector2u SIZE_OF_THE_NEAR_STAR_IMAGE(6, 3);
constexpr std::string_view NEAR_STAR_IMAGE = "backgrounds/space/near_star.png";
constexpr std::size_t FRAMES_OF_THE_NEAR_STAR = 2;
constexpr std::size_t SECOND_FRAME = 1;
constexpr sf::FloatRect RIGHT_HALF_OF_THE_IMAGE({3.0F, 0.0F}, {3.0F, 3.0F});
constexpr std::size_t NO_FRAME = 0;

/** @brief A rectangle of 3 by 3 pixels at (99, 49), textured from the right
 * half of that image: two triangles, six corners, the first its top left and
 * the third its bottom right. */
constexpr sf::FloatRect WHERE_THE_STAR_IS_DRAWN({99.0F, 49.0F}, {3.0F, 3.0F});
constexpr std::size_t CORNERS_OF_TWO_TRIANGLES = 6;
constexpr std::size_t FIRST_CORNER = 0;
constexpr std::size_t THIRD_CORNER = 2;
constexpr sf::Vector2f TOP_LEFT_OF_THE_STAR(99.0F, 49.0F);
constexpr sf::Vector2f BOTTOM_RIGHT_OF_THE_STAR(102.0F, 52.0F);
constexpr sf::Vector2f TOP_LEFT_OF_THE_RIGHT_HALF(3.0F, 0.0F);
constexpr sf::Vector2f BOTTOM_RIGHT_OF_THE_RIGHT_HALF(6.0F, 3.0F);
