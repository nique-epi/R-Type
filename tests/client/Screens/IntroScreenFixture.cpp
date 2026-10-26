#include "IntroScreenFixture.hpp"
#include <memory>
#include <optional>
#include "FakeMusicPlayer.hpp"
#include "IntroScreen.hpp"
#include "IntroScreenParts.hpp"
#include "IntroScreenTestConstants.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::IntroScreen;
using rtype::client::IntroScreenParts;

void IntroScreenFixture::start() {
  intro_ = std::make_unique<IntroScreen>(
      IntroScreenParts{.navigator = navigator_,
                       .settings = settings_,
                       .sky = sky_,
                       .sounds = sounds_,
                       .music = music_,
                       .seed = rtype::client::testing::INTRO_SCREEN_SEED},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

IntroScreen& IntroScreenFixture::intro() { return *intro_; }

void IntroScreenFixture::playFor(rtype::engine::Duration duration) {
  for (rtype::engine::Duration played{}; played < duration;
       played += ONE_FRAME) {
    intro_->update(ONE_FRAME);
  }
}

void IntroScreenFixture::playWithTheMusicFor(rtype::engine::Duration duration) {
  for (rtype::engine::Duration played{}; played < duration;
       played += ONE_FRAME) {
    if (const std::optional<rtype::engine::Duration> offset =
            music_.playingOffset()) {
      music_.setPlayingOffset(*offset + ONE_FRAME);
    }
    intro_->update(ONE_FRAME);
  }
}

void IntroScreenFixture::close() { intro_.reset(); }

const RecordingNavigator& IntroScreenFixture::navigator() const {
  return navigator_;
}

const RecordingSky& IntroScreenFixture::sky() const { return sky_; }

const RecordingSoundPlayer& IntroScreenFixture::sounds() const {
  return sounds_;
}

FakeMusicPlayer& IntroScreenFixture::music() { return music_; }

rtype::client::PlayerSettings& IntroScreenFixture::settings() {
  return settings_;
}
