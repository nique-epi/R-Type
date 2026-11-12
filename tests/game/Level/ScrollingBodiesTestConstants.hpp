#pragma once

/** @brief Where the bodies of the tests start, and how far one tick of a
 * level scrolling at 120 units a second carries them: 2 units, to x 498. */
constexpr float BODY_START_X = 500.0F;
constexpr float BODY_ROW_Y = 300.0F;
constexpr float ONE_TICK_OF_SCROLL = 2.0F;
constexpr float CARRIED_ONE_TICK_X = 498.0F;

/** @brief The travel of a paused scroll. */
constexpr float NO_SCROLL = 0.0F;
