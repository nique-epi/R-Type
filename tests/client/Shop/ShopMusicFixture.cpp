#include "ShopMusicFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Ambience.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "ShopScreen.hpp"
#include "ShopScreenParts.hpp"

namespace rtype::client::testing {

ShopMusicFixture::ShopMusicFixture()
    : service_(settings_, storage_), audio_(sounds_) {
  audio_.setAmbience(Ambience::Space);
  screen_ = std::make_unique<ShopScreen>(
      ShopScreenParts{.navigator = navigator_,
                      .sounds = sounds_,
                      .audio = audio_,
                      .settings = service_,
                      .playfieldPointAt = samePlayfieldPoint},
      std::make_unique<RecordingInterfaceCanvas>(painting_));
}

void ShopMusicFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
}

void ShopMusicFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
}

void ShopMusicFixture::click(sf::Vector2i pixel) {
  screen_->handleEvent(leftPress(pixel * INTERFACE_PIXEL));
}

void ShopMusicFixture::close() { screen_.reset(); }

Ambience ShopMusicFixture::ambience() const { return audio_.ambience(); }

}  // namespace rtype::client::testing
