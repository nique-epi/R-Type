#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "PixelGridConstants.hpp"

namespace rtype::client::testing {

/** @brief Seed of the shots of the squadrons of the tests. */
constexpr std::uint32_t TITLE_SEED = 7;

/** @brief One frame of the title, at 60 frames a second. */
constexpr std::chrono::milliseconds TITLE_FRAME{16};

/** @brief PLAY, and the label of the selected bar, in French. */
constexpr std::string_view FRENCH_PLAY_LABEL = "JOUER";

/** @brief Where the logo lies on the mockup of the title screen. */
constexpr sf::Vector2i LOGO_TOP_LEFT(24, 36);

/** @brief Where the LAN ribbon lies on the mockup of the title screen: one row
 * under the logo, whose bottom is row 36 + 42 = 78. */
constexpr sf::Vector2i LAN_BANNER_TOP_LEFT(24, 79);

/**
 * @brief The middle of the bar of PLAY on the mockup, in logical units of the
 * playfield. The ribbon (16 rows, 1 above it, 4 under it) replaces the 8 rows
 * under the logo, so the bar lies 13 rows lower than without it, at (24, 123),
 * and is 160 x 20 pixels large.
 */
constexpr sf::Vector2i PLAY_BAR_MIDDLE =
    sf::Vector2i(104, 133) * static_cast<int>(BACKGROUND_PIXEL_SIZE);

}  // namespace rtype::client::testing
