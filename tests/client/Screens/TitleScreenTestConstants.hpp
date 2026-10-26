#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "BackgroundConstants.hpp"

namespace rtype::client::testing {

/** @brief Seed of the shots of the squadrons of the tests. */
constexpr std::uint32_t TITLE_SEED = 7;

/** @brief Where the logo lies on the mockup of the title screen. */
constexpr sf::Vector2i LOGO_TOP_LEFT(24, 36);

/**
 * @brief The middle of the bar of PLAY on the mockup, whose bar lies at
 * (24, 110) and is 160 x 20 pixels large, in logical units of the playfield.
 */
constexpr sf::Vector2i PLAY_BAR_MIDDLE =
    sf::Vector2i(104, 120) * static_cast<int>(BACKGROUND_PIXEL_SIZE);

}  // namespace rtype::client::testing
