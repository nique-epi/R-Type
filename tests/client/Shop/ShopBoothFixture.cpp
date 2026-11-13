#include "ShopBoothFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Ambience.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ShopScreen.hpp"
#include "ShopScreenParts.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

ShopBoothFixture::ShopBoothFixture()
    : service_(settings_, storage_), audio_(sounds_) {
  audio_.setAmbience(Ambience::Space);
  screen_ = std::make_unique<ShopScreen>(
      ShopScreenParts{.navigator = navigator_,
                      .sounds = sounds_,
                      .audio = audio_,
                      .settings = service_,
                      .playfieldPointAt = samePlayfieldPoint,
                      .motion = MotionPreference::Full},
      std::make_unique<RecordingInterfaceCanvas>(painting_));
}

void ShopBoothFixture::reopenWithLessMotion() {
  screen_.reset();
  screen_ = std::make_unique<ShopScreen>(
      ShopScreenParts{.navigator = navigator_,
                      .sounds = sounds_,
                      .audio = audio_,
                      .settings = service_,
                      .playfieldPointAt = samePlayfieldPoint,
                      .motion = MotionPreference::Reduced},
      std::make_unique<RecordingInterfaceCanvas>(painting_));
}

void ShopBoothFixture::pointAt(sf::Vector2i pixel) {
  screen_->handleEvent(pointerMove(pixel * INTERFACE_PIXEL));
  screen_->update(engine::Duration::zero());
}

void ShopBoothFixture::wait(engine::Duration elapsed) {
  screen_->update(elapsed);
}

const InterfaceCanvasRecord& ShopBoothFixture::painting() const {
  return painting_;
}

void ShopBoothFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
}

void ShopBoothFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
}

void ShopBoothFixture::click(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel * INTERFACE_PIXEL));
}

void ShopBoothFixture::close() { screen_.reset(); }

Ambience ShopBoothFixture::ambience() const { return audio_.ambience(); }

}  // namespace rtype::client::testing
