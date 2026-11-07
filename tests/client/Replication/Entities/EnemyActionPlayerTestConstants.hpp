#pragma once

#include <chrono>
#include <cstdint>
#include "FrameAnchor.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

/** @brief The network id of the enemy the tests announce actions to, and one
 * the client does not have. */
constexpr std::uint32_t ENEMY_ID = 7;
constexpr std::uint32_t UNKNOWN_ENEMY_ID = 8;

/** @brief The attack the sheet describes, and one it does not. */
constexpr std::uint8_t DESCRIBED_ATTACK = 1;
constexpr std::uint8_t UNDESCRIBED_ATTACK = 2;

/** @brief The idle frame of the enemy, the frames of its strike, and how long
 * each strike frame shows. */
constexpr game::SpriteFrame IDLE_FRAME{
    .left = 0, .top = 0, .width = 8, .height = 8};
constexpr engine::Duration STRIKE_FRAME_TIME = std::chrono::milliseconds(100);
constexpr FrameAnchor STRIKE_ANCHOR{.x = -4, .y = 1};

/** @brief How long the idle frame shows, and the two frames of the strike. */
constexpr engine::Duration IDLE_FRAME_TIME = std::chrono::milliseconds(200);
constexpr game::SpriteFrame FIRST_STRIKE_FRAME{
    .left = 0, .top = 16, .width = 16, .height = 8};
constexpr game::SpriteFrame SECOND_STRIKE_FRAME{
    .left = 18, .top = 16, .width = 16, .height = 8};

}  // namespace rtype::client::testing
