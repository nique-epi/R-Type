#include "BossPresentationFixture.hpp"
#include "BossPresentation.hpp"
#include "EffectTestConstants.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSoundPlayer.hpp"

BossPresentationFixture::BossPresentationFixture()
    : effects_(entities_, components_, EFFECT_TEST_SEED),
      presentation_(effects_.tools(), sounds_, audio_) {}

rtype::client::BossPresentation& BossPresentationFixture::presentation() {
  return presentation_;
}

const RecordingSoundPlayer& BossPresentationFixture::sounds() const {
  return sounds_;
}

const RecordingAudioService& BossPresentationFixture::audio() const {
  return audio_;
}
