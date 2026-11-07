#include "EnemyActionPlayerFixtures.hpp"
#include "ActionAnimation.hpp"
#include "ActionKey.hpp"
#include "EnemyActionPlayerTestConstants.hpp"
#include "EnemyPhase.hpp"
#include "EntityAppearance.hpp"
#include "Playback.hpp"
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
          .anchor = STRIKE_ANCHOR}},
      .enragedIdleFrames = {}};
}

EntityAppearance appearanceWithARage() {
  EntityAppearance appearance = appearanceWithAStrike();
  appearance.enragedIdleFrames = {ENRAGED_IDLE_FRAME};
  appearance.enragedIdleFrameDuration = IDLE_FRAME_TIME;
  appearance.actions.push_back(ActionAnimation{
      .key = game::ActionKey{.attack = RAGE_ATTACK,
                             .phase = game::EnemyPhase::Enraged},
      .frames = {FIRST_RAGE_FRAME, SECOND_RAGE_FRAME},
      .frameDuration = RAGE_FRAME_TIME,
      .playback = Playback::Hold,
      .returnsToIdle = true,
      .anchor = STRIKE_ANCHOR});
  return appearance;
}

}  // namespace rtype::client::testing
