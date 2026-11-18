#pragma once

#include <chrono>
#include <cstdint>

namespace rtype::client::testing {

/**
 * @brief Numbers and the frame of the turns of the rotor that brings each to
 * the top: the frames turn the rotor half a pocket each, 74 a turn, so a
 * number p pockets clockwise from the zero reaches the top 74 - 2p frames
 * on. 17 lies 8 pockets from the zero, 26 lies 36.
 */
constexpr std::uint8_t ZERO_NUMBER = 0;
constexpr int ZERO_STOPS_ON = 0;
constexpr std::uint8_t SEVENTEEN_NUMBER = 17;
constexpr int SEVENTEEN_STOPS_ON = 58;
constexpr std::uint8_t TWENTY_SIX_NUMBER = 26;
constexpr int TWENTY_SIX_STOPS_ON = 2;

/**
 * @brief How many frames the rotor still turns, the slowing steps read from
 * the end: 4 of 200 ms, 4 of 140 ms, 5 of 100 ms, 6 of 70 ms, then 50 ms
 * each. 200 ms is one frame; 800 ms the four last; one nanosecond more
 * begins a fifth; 6 s are the 19 slowing frames, 2.28 s, then 3.72 s of
 * 50 ms, 75 frames begun.
 */
constexpr std::chrono::milliseconds ONE_LAST_FRAME{200};
constexpr int ONE_FRAME_LEFT = 1;
constexpr std::chrono::milliseconds FOUR_LAST_FRAMES{800};
constexpr int FOUR_FRAMES_LEFT = 4;
constexpr std::chrono::nanoseconds JUST_PAST_FOUR_FRAMES{800'000'001};
constexpr int FIVE_FRAMES_LEFT = 5;
constexpr std::chrono::seconds A_WHOLE_SPIN{6};
constexpr int FRAMES_OF_A_WHOLE_SPIN = 94;

/** @brief The middle of the wheel, 125 pixels wide; the ball sprite is
 * centered 2 pixels in from its top-left corner; the pocket of the ball lies
 * 26 pixels from the middle, the track 48; and how far rounding may move it. */
constexpr double WHEEL_MIDDLE = 62.5;
constexpr int BALL_SPRITE_CENTER = 2;
constexpr double POCKET_RADIUS = 26.0;
constexpr double TRACK_RADIUS = 48.0;
constexpr double ROUNDING = 1.0;

}  // namespace rtype::client::testing
