#pragma once

#include <chrono>
#include <cstdint>
#include <vector>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "EntityAppearance.hpp"
#include "FrameAnchor.hpp"
#include "Playback.hpp"
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

/** @returns The appearance of the enemy: one idle frame, and the strike of
 * DESCRIBED_ATTACK in two frames, held, with an anchor, then back to idle. */
inline EntityAppearance appearanceWithAStrike() {
  return EntityAppearance{
      .sheet = "sprites/enemy.png",
      .idleFrames = {IDLE_FRAME},
      .idleFrameDuration = std::chrono::milliseconds(200),
      .actions = {ActionAnimation{
          .key = game::ActionKey{.attack = DESCRIBED_ATTACK,
                                 .phase = game::EnemyPhase::Strike},
          .frames = {game::SpriteFrame{
                         .left = 0, .top = 16, .width = 16, .height = 8},
                     game::SpriteFrame{
                         .left = 18, .top = 16, .width = 16, .height = 8}},
          .frameDuration = STRIKE_FRAME_TIME,
          .playback = Playback::Hold,
          .returnsToIdle = true,
          .anchor = STRIKE_ANCHOR}}};
}

}  // namespace rtype::client::testing
