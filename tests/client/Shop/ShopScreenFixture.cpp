#include "ShopScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include "KeyPress.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "ShopScreen.hpp"
#include "ShopScreenParts.hpp"

namespace rtype::client::testing {

ShopScreenFixture::ShopScreenFixture()
    : service_(settings_, storage_),
      screen_(std::make_unique<ShopScreen>(
          ShopScreenParts{.navigator = navigator_,
                          .sounds = sounds_,
                          .settings = service_,
                          .playfieldPointAt = samePlayfieldPoint},
          std::make_unique<RecordingInterfaceCanvas>(painting_))) {}

void ShopScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::pointAt(sf::Vector2i pixel, bool clicked) {
  const sf::Vector2i windowPixel = pixel * INTERFACE_PIXEL;
  if (clicked) {
    screen_->handleEvent(leftPress(windowPixel));
    screen_->handleEvent(leftRelease(windowPixel));
  } else {
    screen_->handleEvent(pointerMove(windowPixel));
  }
  screen_->update(ONE_FRAME);
}

void ShopScreenFixture::give(std::uint32_t kopecks) {
  settings_.kopecks = kopecks;
  screen_->update(ONE_FRAME);
}

const PlayerSettings& ShopScreenFixture::settings() const { return settings_; }

const RecordingSettingsStorage& ShopScreenFixture::storage() const {
  return storage_;
}

const RecordingNavigator& ShopScreenFixture::navigator() const {
  return navigator_;
}

const RecordingSoundPlayer& ShopScreenFixture::sounds() const {
  return sounds_;
}

const InterfaceCanvasRecord& ShopScreenFixture::painting() const {
  return painting_;
}

}  // namespace rtype::client::testing
