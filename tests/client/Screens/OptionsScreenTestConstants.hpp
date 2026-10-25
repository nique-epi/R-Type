#pragma once

#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <cstddef>
#include "PixelSize.hpp"

namespace rtype::client::testing {

/** @brief A 1920 x 1080 desktop: 960, 1280 and 1600 wide fit, 1600 x 900
 * is the window. */
constexpr PixelSize DESKTOP{.width = 1920, .height = 1080};
constexpr std::size_t LARGEST_FIT = 2;

/** @brief The second size of the list, which the tests take from the window
 * of 1600 x 900. */
constexpr PixelSize WINDOW_1280_BY_720{.width = 1280, .height = 720};
constexpr std::size_t INDEX_OF_1280_BY_720 = 1;

/** @brief The picture the team chose: SAVED at (270, 36); the description
 * from x 112 to the safe margin at 456; the open list of sizes from x 332;
 * the label of the CONTROLS tab at (158, 55); the left end of the track of
 * MUSIC at (281, 78). */
constexpr sf::Vector2i SAVED_ANCHOR(270, 36);
constexpr int DESCRIPTION_LEFT = 112;
constexpr int DESCRIPTION_RIGHT = 456;
constexpr int OPEN_LIST_LEFT = 332;
constexpr sf::Vector2i CONTROLS_TAB(158, 55);
constexpr sf::Vector2i MUSIC_TRACK_START(281, 78);
constexpr sf::Vector2i ALONG_THE_TRACK(80, 0);

/** @brief The MUSIC volume of a first launch, one step of 5 above it, and a
 * quiet volume away from it. */
constexpr int MUSIC_DEFAULT = 70;
constexpr int MUSIC_ONE_STEP_UP = 75;
constexpr int QUIET_MUSIC = 10;

/** @brief The four tabs, the longest holding eight rows. */
constexpr std::size_t ROWS_IN_THE_LONGEST_TAB = 8;
constexpr int TABS = 4;

/** @brief SAVED shows for a second; NOT SAVED is still there ten seconds
 * after a failed save. */
constexpr std::chrono::seconds SAVED_STAMP_TIME(1);
constexpr std::chrono::seconds LONG_AFTER_A_FAILED_SAVE(10);

}  // namespace rtype::client::testing
