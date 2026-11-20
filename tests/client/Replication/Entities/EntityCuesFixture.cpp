#include "EntityCuesFixture.hpp"
#include <string>
#include <vector>
#include "AppearanceReference.hpp"
#include "Effects/EffectTestConstants.hpp"
#include "EntityCues.hpp"
#include "EntityCuesTestConstants.hpp"
#include "EntityFeedback.hpp"
#include "EntitySounds.hpp"
#include "EntityType.hpp"
#include "Position.hpp"

using rtype::client::AppearanceReference;
using rtype::client::EntityFeedback;
using rtype::client::EntitySounds;
using rtype::client::testing::CUED_APPEAR_SOUND;
using rtype::client::testing::CUED_BYDO_PLACE;
using rtype::client::testing::CUED_DEATH_SOUND;

EntityCuesFixture::EntityCuesFixture()
    : appearance_{.sheet = "sprites/bydo.png",
                  .idleFrames = {},
                  .idleFrameDuration = {},
                  .actions = {},
                  .enragedIdleFrames = {},
                  .deathFrames = {},
                  .deathFramesWithoutFlash = {},
                  .effects = {},
                  .looks = {},
                  .bossName = {},
                  .sounds = EntitySounds{.appear = CUED_APPEAR_SOUND,
                                         .death = CUED_DEATH_SOUND,
                                         .actions = {}}},
      effects_(entities_, components_, EFFECT_TEST_SEED),
      bydo_(entities_.create()) {
  components_.add(bydo_, rtype::game::EntityType::Enemy);
  components_.add(bydo_, CUED_BYDO_PLACE);
  components_.add(bydo_, AppearanceReference{.appearance = &appearance_});
}

void EntityCuesFixture::announceTheBydo() {
  rtype::client::announce(
      EntityFeedback{.effects = effects_, .sounds = sounds_}, components_,
      bydo_);
}

void EntityCuesFixture::killTheBydo() {
  rtype::client::explode(EntityFeedback{.effects = effects_, .sounds = sounds_},
                         components_, bydo_);
}

const std::vector<std::string>& EntityCuesFixture::played() const {
  return sounds_.played();
}
