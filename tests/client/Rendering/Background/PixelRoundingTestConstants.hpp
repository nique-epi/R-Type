#pragma once

/** @brief Positions a quarter of a pixel right of a whole column, a quarter
 * left of one, on one, and what each lies right of its pixel. */
constexpr float A_QUARTER_PAST_TWO = 2.25F;
constexpr float A_QUARTER_SHORT_OF_ZERO = -0.25F;
constexpr float ON_THREE = 3.0F;
constexpr float A_QUARTER_OF_A_PIXEL = 0.25F;
constexpr float THREE_QUARTERS = 0.75F;
constexpr float NOTHING = 0.0F;

/** @brief A background enlarged four times, and twice. */
constexpr unsigned int FOUR_TIMES = 4;
constexpr unsigned int TWICE = 2;

/** @brief A plane shifted three quarters of a pixel left, which covers three
 * pixels left of the background enlarged four times; and a plane shifted
 * 0.3 of a pixel right, 0.6 of a pixel of the background enlarged twice,
 * which rounds to one. */
constexpr float THREE_QUARTERS_LEFT = -0.75F;
constexpr int THREE_ENLARGED_PIXELS_LEFT = -3;
constexpr float THREE_TENTHS_RIGHT = 0.3F;
constexpr int ONE_ENLARGED_PIXEL_RIGHT = 1;
