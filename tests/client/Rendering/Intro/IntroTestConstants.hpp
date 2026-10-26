#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>

namespace rtype::client::testing {

/** @brief Seed of the casts the tests build. */
constexpr std::uint32_t INTRO_TEST_SEED = 11;

/** @brief Seconds of the music after a moment, small enough to stay in the
 * same shot and step of an animation; half a beat of 60/109 s. */
constexpr float A_MOMENT = 0.01F;
constexpr float LONGER_THAN_A_FLASH = 0.03F;
constexpr float HALF_A_BEAT = 30.0F / 109.0F;

/** @brief The first freeze holds a kill of the game, 50 ms; the scene is
 * 50 ms behind the music once it is over. */
constexpr float FIRST_FREEZE = 0.05F;

/** @brief Where the close-up of the second pilot looks: centered on
 * (240, 155), 160 by 90 pixels of the world, as the mockup frames it. */
constexpr sf::Vector2i SECOND_CLOSEUP_TOP_LEFT(160, 110);

/** @brief Where a ship is in the middle of its close-up: centered on the
 * shot, 240 less half of its 32 pixels. */
constexpr float MIDDLE_OF_A_CLOSEUP_X = 224.0F;

/** @brief The ring bursts 30 ms after the beams leave, as the mockup
 * times it. */
constexpr float BURST_AFTER_THE_BEAMS = 0.03F;

/** @brief The width of the screen, in pixels of the world: past it, debris
 * are gone. */
constexpr float SCREEN_WIDTH = 480.0F;

/** @brief Where the squadron enters from, in the mockup. */
constexpr float SQUADRON_ENTRY_X = -70.0F;

/** @brief The rows of the pink and green ships before their swap, 48 pixels
 * apart, and how far a bob may move each of them. */
constexpr float SWAPPED_ROWS_GAP = 48.0F;
constexpr float TWO_BOBS = 2.0F;

/** @brief How many sounds of each kind the mockup plays: one flyby per
 * close-up, 17 shots of players (the prey and the 16 of the cascade), 3 of
 * enemies, 17 small explosions. */
constexpr std::size_t FLYBYS = 4;
constexpr std::size_t PLAYER_SHOTS = 17;
constexpr std::size_t ENEMY_SHOTS = 3;
constexpr std::size_t SMALL_EXPLOSIONS = 17;

/** @brief The logo lands 0.3 s after the ninth beat of 60/109 s. */
constexpr float LOGO_LANDS = (9.0F * 60.0F / 109.0F) + 0.3F;

/** @brief Forty speed lines at full warp; a speed line is a rectangle one
 * pixel high and at least 16 long. */
constexpr std::size_t FULL_SPEED_LINES = 40;
constexpr int SHORTEST_SPEED_LINE = 16;

/** @brief The scale of a close-up and of the wide shot. */
constexpr int CLOSEUP_SCALE = 3;
constexpr int WIDE_SCALE = 1;

/** @brief A press 1.23 s into the music, between its second and third beats,
 * whose next beat is the third, 3 × 60/109 s; a press just before the slam,
 * nine beats in; how far a time computed in float may be from its value. */
constexpr float MUSIC_AT_A_PRESS = 1.23F;
constexpr float NEXT_BEAT_AFTER_THE_PRESS = 3.0F * 60.0F / 109.0F;
constexpr float JUST_BEFORE_THE_SLAM = 4.9F;
constexpr float FLOAT_ROUNDING = 1.0e-5F;

/** @brief A press 50 ms before the first beat of the music, which the
 * position counts from: the logo then waits those 50 ms. */
constexpr float BEFORE_THE_FIRST_BEAT = -0.05F;
constexpr float UNTIL_THE_FIRST_BEAT = 0.05F;

}  // namespace rtype::client::testing
