#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include "CasinoSpriteConstants.hpp"
#include "ProtocolConstants.hpp"
#include "TimeConstants.hpp"
#include "WheelSlowdownStep.hpp"

namespace rtype::client {

/** @brief The pockets of a single zero wheel, clockwise from the zero, as the
 * rotor of wheel_rotor_turn.png draws them, the zero at the top of frame 0. */
constexpr std::array<std::uint8_t, network::HIGHEST_ROULETTE_NUMBER + 1>
    WHEEL_ORDER{0,  32, 15, 19, 4,  21, 2,  25, 17, 34, 6, 27, 13,
                36, 11, 30, 8,  23, 10, 5,  24, 16, 33, 1, 20, 14,
                31, 9,  22, 18, 29, 7,  28, 12, 35, 3,  26};

/** @brief The frames of the turns of the rotor per pocket: each frame turns
 * it half a pocket further. */
constexpr int ROTOR_FRAMES_PER_POCKET =
    WHEEL_ROTOR_TURN_FRAMES / static_cast<int>(WHEEL_ORDER.size());
static_assert(ROTOR_FRAMES_PER_POCKET * static_cast<int>(WHEEL_ORDER.size()) ==
                  WHEEL_ROTOR_TURN_FRAMES,
              "the turns of the rotor end where they began");

/**
 * @brief How the rotor slows down to stop: from the end of the spin back,
 * the steps of ROTOR_SLOWDOWN, each a few frames of a longer time; before
 * them, a frame every ROTOR_FASTEST_FRAME_TIME.
 */
constexpr engine::Duration ROTOR_FASTEST_FRAME_TIME =
    std::chrono::milliseconds(50);
constexpr std::array ROTOR_SLOWDOWN{
    WheelSlowdownStep{.frameTime = std::chrono::milliseconds(200), .frames = 4},
    WheelSlowdownStep{.frameTime = std::chrono::milliseconds(140), .frames = 4},
    WheelSlowdownStep{.frameTime = std::chrono::milliseconds(100), .frames = 5},
    WheelSlowdownStep{.frameTime = std::chrono::milliseconds(70), .frames = 6}};

/**
 * @brief The ball: it runs round the track against the rotor,
 * BALL_TURNS_PER_SECOND turns a second, BALL_TRACK_RADIUS pixels from the
 * middle of the wheel, until the rotor has BALL_DROP_FRAMES frames left to
 * turn; then it lies in the winning pocket, BALL_POCKET_RADIUS from the
 * middle, and turns with it. BALL_CENTER is the pixel of its sprite on its
 * path.
 */
constexpr double BALL_TURNS_PER_SECOND = 1.5;
constexpr double BALL_TRACK_RADIUS = 48.0;
constexpr double BALL_POCKET_RADIUS = 26.0;
constexpr int BALL_DROP_FRAMES = 8;
constexpr int BALL_CENTER = 2;

/** @brief A whole turn of the wheel, and the angle of its top, where the ball
 * stops, in radians clockwise from the right as the screen shows them. */
constexpr double FULL_TURN_RADIANS = 2.0 * std::numbers::pi;
constexpr double TOP_OF_THE_WHEEL_RADIANS = -std::numbers::pi / 2.0;

}  // namespace rtype::client
