#include "EnemyActionPlayerFixtures.hpp"
#include <chrono>
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EnemyPhase.hpp"
#include "EntityAppearance.hpp"
#include "Playback.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

namespace {

constexpr engine::Duration IDLE_FRAME_TIME = std::chrono::milliseconds(200);
constexpr game::SpriteFrame FIRST_STRIKE_FRAME{
    .left = 0, .top = 16, .width = 16, .height = 8};
constexpr game::SpriteFrame SECOND_STRIKE_FRAME{
    .left = 18, .top = 16, .width = 16, .height = 8};

}  // namespace

EntityAppearance appearanceWithAStrike() {
  return EntityAppearance{
      .sheet = "sprites/enemy.png",
      .idleFrames = {IDLE_FRAME},
      .idleFrameDuration = IDLE_FRAME_TIME,
      .actions = {ActionAnimation{
          .key = game::ActionKey{.attack = DESCRIBED_ATTACK,
                                 .phase = game::EnemyPhase::Strike},
          .frames = {FIRST_STRIKE_FRAME, SECOND_STRIKE_FRAME},
          .frameDuration = STRIKE_FRAME_TIME,
          .playback = Playback::Hold,
          .returnsToIdle = true,
          .anchor = STRIKE_ANCHOR}}};
}

}  // namespace rtype::client::testing
