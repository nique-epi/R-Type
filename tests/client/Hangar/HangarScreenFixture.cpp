#include "HangarScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "HangarScreen.hpp"
#include "HangarScreenParts.hpp"
#include "HangarScreenTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"

namespace rtype::client::testing {

HangarScreenFixture::HangarScreenFixture()
    : HangarScreenFixture(MotionPreference::Full) {}

HangarScreenFixture::HangarScreenFixture(MotionPreference motion)
    : service_(settings_, storage_),
      screen_(std::make_unique<HangarScreen>(
          HangarScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .audio = audio_,
                            .settings = service_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .palette = PLAYER_COLORS,
                            .shipColor = FIRST_PILOT_COLOR,
                            .motion = motion},
          std::make_unique<RecordingInterfaceCanvas>(painting_))) {}

void HangarScreenFixture::holdEveryItem() { settings_.shipLocker.items.set(); }

void HangarScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(sf::Event::KeyPressed{.code = key});
  screen_->handleEvent(sf::Event::KeyReleased{.code = key});
  update(ONE_FRAME);
}

void HangarScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(sf::Event::KeyPressed{.code = key});
  update(ONE_FRAME);
}

void HangarScreenFixture::clickAt(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel * INTERFACE_PIXEL));
  screen_->handleEvent(leftRelease(pixel * INTERFACE_PIXEL));
  update(ONE_FRAME);
}

void HangarScreenFixture::update(engine::Duration elapsed) {
  screen_->update(elapsed);
}

const PlayerSettings& HangarScreenFixture::settings() const {
  return settings_;
}

const RecordingSettingsStorage& HangarScreenFixture::storage() const {
  return storage_;
}

const RecordingNavigator& HangarScreenFixture::navigator() const {
  return navigator_;
}

const RecordingSoundPlayer& HangarScreenFixture::sounds() const {
  return sounds_;
}

const RecordingAudioService& HangarScreenFixture::audio() const {
  return audio_;
}

const InterfaceCanvasRecord& HangarScreenFixture::painting() const {
  return painting_;
}

}  // namespace rtype::client::testing
