#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <string_view>
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

/** @brief Network ids checked for their loop start, from 0 included. */
constexpr std::uint32_t CHECKED_NETWORK_IDS = 10000;

/** @brief Two network ids that follow each other, as the server gives two
 * Bydos of a wave. */
constexpr std::uint32_t FIRST_BYDO_ID = 1;
constexpr std::uint32_t NEXT_BYDO_ID = 2;

/** @brief The appearance the tests draw: two frames of a row of a test sheet,
 * 250 ms each, as a Bydo of the entity sheet shows. */
constexpr std::string_view TEST_SHEET_ID = "sprites/test.png";
constexpr std::array TEST_IDLE_FRAMES{
    rtype::game::SpriteFrame{.left = 0, .top = 72, .width = 24, .height = 24},
    rtype::game::SpriteFrame{.left = 26, .top = 72, .width = 24, .height = 24}};
constexpr rtype::engine::Duration TEST_FRAME_TIME =
    std::chrono::milliseconds(250);
