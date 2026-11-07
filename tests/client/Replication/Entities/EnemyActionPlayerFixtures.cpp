#include "EnemyActionPlayerFixtures.hpp"
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EnemyPhase.hpp"
#include "EntityAppearance.hpp"
#include "Playback.hpp"
#include "SpriteFrame.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

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
