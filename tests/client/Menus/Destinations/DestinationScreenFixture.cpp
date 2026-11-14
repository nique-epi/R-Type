#include "DestinationScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "DestinationScreen.hpp"
#include "DestinationScreenParts.hpp"
#include "DestinationScreenTestConstants.hpp"
#include "PlayStart.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenInputs.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::DestinationScreen;
using rtype::client::DestinationScreenParts;
using rtype::client::PlayStart;
using rtype::client::testing::BALANCE;

DestinationScreenFixture::DestinationScreenFixture() {
  settings_.kopecks = BALANCE;
  reopenWith(PlayStart::ServerMap);
}

void DestinationScreenFixture::reopenWith(PlayStart playStart) {
  screen_.reset();
  screen_ = std::make_unique<DestinationScreen>(
      DestinationScreenParts{.navigator = navigator_,
                             .sounds = sounds_,
                             .settings = settings_,
                             .playfieldPointAt = samePlayfieldPoint,
                             .playStart = playStart},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void DestinationScreenFixture::press(sf::Keyboard::Key key) {
  tapKey(*screen_, key);
  screen_->update(ONE_FRAME);
}

void DestinationScreenFixture::clickAt(sf::Vector2i interfacePixel) {
  clickInterfacePixel(*screen_, interfacePixel);
  screen_->update(ONE_FRAME);
}

DestinationScreen& DestinationScreenFixture::screen() { return *screen_; }

rtype::client::PlayerSettings& DestinationScreenFixture::settings() {
  return settings_;
}

const RecordingNavigator& DestinationScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& DestinationScreenFixture::canvas() const {
  return canvas_;
}

const RecordingSoundPlayer& DestinationScreenFixture::sounds() const {
  return sounds_;
}
