#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <string_view>

namespace rtype::client::testing {

/** @brief A score of five digits ending at (100, 10): two zeros pad it, 8
 * pixels a digit, 16 at scale 2; a score above 9999999 stops there. */
constexpr std::uint32_t FIVE_DIGIT_SCORE = 12450;
constexpr std::string_view TWO_PADDING_ZEROS = "00";
constexpr sf::Vector2i SCORE_END(100, 10);
constexpr sf::Vector2i ZEROS_END(60, 10);
constexpr sf::Vector2i SCALED_ZEROS_END(20, 10);
constexpr int DOUBLE_SCALE = 2;
constexpr std::uint32_t EIGHT_DIGIT_SCORE = 12345678;
constexpr std::string_view LARGEST_SHOWN = "9999999";
constexpr sf::Color DIGIT_COLOR = sf::Color::White;
constexpr sf::Color ZERO_COLOR = sf::Color::Blue;

}  // namespace rtype::client::testing
