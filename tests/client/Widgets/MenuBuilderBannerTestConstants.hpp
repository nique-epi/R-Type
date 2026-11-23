#pragma once

#include <SFML/System/Vector2.hpp>
#include <string_view>

namespace rtype::client::testing {

/** @brief A page of the title: its version at row 24, its logo of 210 x 42 at
 * row 24 + 8 + 4 = 36, so the bottom of the logo is row 78. */
constexpr std::string_view VERSION_OVER_THE_BANNER = "VER";
constexpr std::string_view LOGO_OVER_THE_BANNER = "logo.png";
constexpr sf::Vector2i SIZE_OF_THE_LOGO_OVER_THE_BANNER(210, 42);

/** @brief The banner of that page, 210 x 16, as the LAN ribbon of the
 * mockup: one row under the logo, at row 79, and the subtitle four rows under
 * it, at row 79 + 16 + 4 = 99. */
constexpr std::string_view BANNER_UNDER_THE_LOGO = "banner.png";
constexpr sf::Vector2i SIZE_OF_THE_BANNER(210, 16);
constexpr sf::Vector2i WHERE_THE_BANNER_LIES(24, 79);
constexpr std::string_view SUBTITLE_UNDER_THE_BANNER = "SUBTITLE";
constexpr sf::Vector2i WHERE_THE_SUBTITLE_FOLLOWS_THE_BANNER(24, 99);

}  // namespace rtype::client::testing
