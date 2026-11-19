#include "BossPresentationFixture.hpp"
#include <string>
#include "AppearanceReference.hpp"
#include "BossPresentation.hpp"
#include "BossPresentationTestConstants.hpp"
#include "EffectTestConstants.hpp"
#include "Entity.hpp"
#include "EntityAppearance.hpp"
#include "NetworkIdentity.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSoundPlayer.hpp"

BossPresentationFixture::BossPresentationFixture()
    : effects_(entities_, components_, EFFECT_TEST_SEED),
      presentation_(effects_.tools(), sounds_, audio_),
      namedBossAppearance_{.sheet = {},
                           .idleFrames = {},
                           .idleFrameDuration = {},
                           .actions = {},
                           .enragedIdleFrames = {},
                           .deathFrames = {},
                           .deathFramesWithoutFlash = {},
                           .effects = {},
                           .looks = {},
                           .bossName = std::string{PRESENTED_BOSS_NAME}} {}

void BossPresentationFixture::placeNamedBoss() {
  const rtype::engine::Entity boss = entities_.create();
  components_.add(boss,
                  rtype::game::NetworkIdentity{.networkId = PRESENTED_BOSS_ID});
  components_.add(boss, rtype::client::AppearanceReference{
                            .appearance = &namedBossAppearance_});
}

rtype::client::BossPresentation& BossPresentationFixture::presentation() {
  return presentation_;
}

const RecordingSoundPlayer& BossPresentationFixture::sounds() const {
  return sounds_;
}

const RecordingAudioService& BossPresentationFixture::audio() const {
  return audio_;
}
