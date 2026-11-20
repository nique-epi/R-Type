#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <numbers>
#include "TimeConstants.hpp"

namespace rtype::client {

/**
 * @brief The bulbs of the casino chase: one in CASINO_CHASE_PHASES is out at
 * a time, and the pattern moves one rank every CASINO_CHASE_STEP.
 */
constexpr std::size_t CASINO_CHASE_PHASES = 3;
constexpr engine::Duration CASINO_CHASE_STEP = std::chrono::milliseconds(120);

/**
 * @brief A lit bulb, as the pictures of the casino paint it: a pale cream or
 * yellow pixel, its red above LIT_BULB_RED_FLOOR, its green above
 * LIT_BULB_GREEN_FLOOR and its blue above LIT_BULB_BLUE_FLOOR; the gold of
 * the frames around the bulbs stays under one of them.
 */
constexpr std::uint8_t LIT_BULB_RED_FLOOR = 220;
constexpr std::uint8_t LIT_BULB_GREEN_FLOOR = 185;
constexpr std::uint8_t LIT_BULB_BLUE_FLOOR = 110;

/** @brief The weights of red, green and blue in the brightness of a pixel
 * (ITU-R BT.601). */
constexpr float LUMINANCE_RED_WEIGHT = 0.299F;
constexpr float LUMINANCE_GREEN_WEIGHT = 0.587F;
constexpr float LUMINANCE_BLUE_WEIGHT = 0.114F;

/** @brief The shifts and factors of the scrambling of a number, those of the
 * "lowbias32" hash of Chris Wellons, whose output bits each depend on every
 * input bit. */
constexpr unsigned SCRAMBLE_FIRST_SHIFT = 16;
constexpr unsigned SCRAMBLE_SECOND_SHIFT = 15;
constexpr std::uint32_t SCRAMBLE_FIRST_FACTOR = 0x7feb352dU;
constexpr std::uint32_t SCRAMBLE_SECOND_FACTOR = 0x846ca68bU;

/** @brief A whole turn, in radians: the beats of the lights swing over
 * one. */
constexpr float FULL_TURN_RADIANS = 2.0F * std::numbers::pi_v<float>;

/** @brief How far the middle of a pixel lies from its top-left corner, on
 * each axis. */
constexpr float PIXEL_MIDDLE = 0.5F;

}  // namespace rtype::client
