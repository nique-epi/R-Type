#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <string_view>

namespace rtype::client::testing {

/** @brief Where the texts of the bottom band and the logo go, as the mockup
 * lays them: the credit and the names at (24, 254), PRESS ANY KEY
 * right-aligned at 456 on the band, then on the footer row 241; the logo at
 * the place of the title, (24, 36). */
constexpr sf::Vector2i BAND_TEXT(24, 254);
constexpr sf::Vector2i HINT_ON_THE_BAND(456, 254);
constexpr sf::Vector2i HINT_ON_THE_FOOTER(456, 241);
constexpr sf::Vector2i LOGO_ON_THE_TITLE(24, 36);

/** @brief The underline of LUKAS: five letters of 8 pixels, on row 264. */
constexpr sf::Vector2i UNDERLINE_TOP_LEFT(24, 264);
constexpr sf::Vector2i LUKAS_UNDERLINE(40, 1);

/** @brief Moments of the tests, in seconds of the music: early in the credit
 * (0.2 s, six letters typed), well into the second close-up, during the
 * bands (4 s), just before PRESS ANY KEY. */
constexpr float INTO_THE_CREDIT = 0.2F;
constexpr std::string_view CREDIT_AT_ITS_START = "R-TYPE";
constexpr float INTO_A_CLOSEUP = 0.3F;
constexpr float DURING_THE_BANDS = 4.0F;
constexpr float BEFORE_THE_HINT = 1.0F;
constexpr float A_MOMENT = 0.01F;

/** @brief The two bands, 24 pixels high, at the top and at the bottom of
 * the 480 × 270 picture. */
constexpr sf::Vector2i BAND_SIZE(480, 24);
constexpr sf::Vector2i BOTTOM_BAND_TOP_LEFT(0, 246);

/** @brief The logo of the title, as Screens/ScreenConstants.hpp names it. */
constexpr std::string_view TITLE_LOGO_ID = "sprites/logo.png";
constexpr sf::Vector2i TITLE_LOGO_SIZE(210, 42);

/** @brief The nickname of the second pilot; one image, the logo alone; four,
 * the logo and its three copies. */
constexpr std::string_view SECOND_PILOT_NAME = "LUKAS";
constexpr std::size_t ONE_IMAGE = 1;
constexpr std::size_t LOGO_AND_ITS_TRAIL = 4;

}  // namespace rtype::client::testing
