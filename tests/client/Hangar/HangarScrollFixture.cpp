#include "HangarScrollFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <string>
#include "HangarScreen.hpp"
#include "HangarScreenParts.hpp"
#include "HangarScreenTestConstants.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "ShipLocker.hpp"

namespace rtype::client::testing {

HangarScrollFixture::HangarScrollFixture()
    : service_(settings_, storage_),
      screen_(std::make_unique<HangarScreen>(
          HangarScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .audio = audio_,
                            .settings = service_,
                            .account = account_,
                            .wallet = wallet_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .palette = PLAYER_COLORS,
                            .shipColor = FIRST_PILOT_COLOR,
                            .motion = MotionPreference::Full},
          std::make_unique<RecordingInterfaceCanvas>(painting_))) {}

void HangarScrollFixture::press(sf::Keyboard::Key key, int times) {
  for (int pressed = 0; pressed < times; ++pressed) {
    screen_->handleEvent(keyPress(key));
    screen_->handleEvent(keyRelease(key));
    screen_->update(ONE_FRAME);
  }
}

void HangarScrollFixture::wheelDownOver(sf::Vector2i pixel) {
  screen_->handleEvent(
      sf::Event::MouseWheelScrolled{.wheel = sf::Mouse::Wheel::Vertical,
                                    .delta = ONE_NOTCH_DOWN,
                                    .position = pixel * INTERFACE_PIXEL});
  screen_->update(ONE_FRAME);
}

void HangarScrollFixture::clickAt(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel * INTERFACE_PIXEL));
  screen_->handleEvent(leftRelease(pixel * INTERFACE_PIXEL));
  screen_->update(ONE_FRAME);
}

void HangarScrollFixture::holdEveryItem() {
  account_.signInAs(HANGAR_ACCOUNT_ID, std::string{HANGAR_TOKEN});
  ShipLocker everyItem;
  everyItem.items.set();
  wallet_.looks = everyItem;
}

const PlayerSettings& HangarScrollFixture::settings() const {
  return settings_;
}

const RecordingSoundPlayer& HangarScrollFixture::sounds() const {
  return sounds_;
}

const InterfaceCanvasRecord& HangarScrollFixture::painting() const {
  return painting_;
}

}  // namespace rtype::client::testing
