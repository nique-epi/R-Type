#include "CasinoEntranceScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include <vector>
#include "CasinoEntranceScreen.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "EntranceLights.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"

using rtype::client::CasinoEntranceScreen;
using rtype::client::CasinoEntranceScreenParts;
using rtype::client::EntranceLights;
using rtype::client::MotionPreference;

CasinoEntranceScreenFixture::CasinoEntranceScreenFixture() {
  reopenWith(MotionPreference::Full);
}

void CasinoEntranceScreenFixture::reopenWith(MotionPreference motion) {
  screen_ = std::make_unique<CasinoEntranceScreen>(
      CasinoEntranceScreenParts{.navigator = navigator_,
                                .sounds = sounds_,
                                .audio = audio_,
                                .playfieldPointAt = samePlayfieldPoint,
                                .lights = EntranceLights{},
                                .motion = motion},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void CasinoEntranceScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::wait(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

const RecordingNavigator& CasinoEntranceScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& CasinoEntranceScreenFixture::canvas() const {
  return canvas_;
}

const std::vector<std::string>& CasinoEntranceScreenFixture::played() const {
  return sounds_.played();
}

const RecordingAudioService& CasinoEntranceScreenFixture::audio() const {
  return audio_;
}
