#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>

namespace rtype::client::testing {

/** @brief A dialog holding an illustration of 240 x 20 and BACK: its buttons
 * stand 12 + 20 + 16 = 48 down its panel, which ends 10 under them, 78 tall,
 * at (104, 96), (480 - 272) / 2 and (270 - 78) / 2. The illustration lies
 * where the heading goes, 16 in and 12 down; BACK, alone, has its bar at
 * x 104 + (272 - 64) / 2 = 208, its label 12 in and 6 down. */
constexpr sf::Vector2i DIALOG_PICTURE_SIZE(240, 20);
constexpr sf::IntRect DIALOG_PICTURE_AREA({120, 108}, DIALOG_PICTURE_SIZE);
constexpr sf::Vector2i BACK_UNDER_THE_PICTURE(220, 150);
constexpr std::string_view BACK_OF_THE_PICTURE = "BACK";

}  // namespace rtype::client::testing
