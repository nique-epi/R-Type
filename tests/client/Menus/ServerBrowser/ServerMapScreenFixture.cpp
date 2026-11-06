#include "ServerMapScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingGameCatalog.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerCatalog.hpp"
#include "ServerChoice.hpp"
#include "ServerListSnapshot.hpp"
#include "ServerMapScreen.hpp"
#include "ServerMapScreenParts.hpp"
#include "ServerMapScreenTestConstants.hpp"
#include "TimeConstants.hpp"

namespace rtype::client::testing {

void ServerMapScreenFixture::openOn(const ServerListSnapshot& list) {
  servers_.show(list);
  screen_ = std::make_unique<ServerMapScreen>(
      ServerMapScreenParts{.navigator = navigator_,
                           .sounds = sounds_,
                           .servers = servers_,
                           .games = games_,
                           .choice = choice_,
                           .playfieldPointAt = samePlayfieldPoint,
                           .motion = MotionPreference::Reduced},
      std::make_unique<RecordingInterfaceCanvas>(painting_));
  update(ONE_FRAME);
}

void ServerMapScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  update(ONE_FRAME);
}

void ServerMapScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  update(ONE_FRAME);
}

void ServerMapScreenFixture::pointAt(sf::Vector2i pixel, bool clicked) {
  const sf::Vector2i windowPixel = pixel * INTERFACE_PIXEL;
  if (clicked) {
    screen_->handleEvent(leftPress(windowPixel));
    screen_->handleEvent(leftRelease(windowPixel));
  } else {
    screen_->handleEvent(pointerMove(windowPixel));
  }
  update(ONE_FRAME);
}

void ServerMapScreenFixture::update(engine::Duration elapsed) {
  screen_->update(elapsed);
}

RecordingServerCatalog& ServerMapScreenFixture::servers() { return servers_; }

RecordingGameCatalog& ServerMapScreenFixture::games() { return games_; }

const RecordingNavigator& ServerMapScreenFixture::navigator() const {
  return navigator_;
}

const ServerChoice& ServerMapScreenFixture::choice() const { return choice_; }

const InterfaceCanvasRecord& ServerMapScreenFixture::painting() const {
  return painting_;
}

}  // namespace rtype::client::testing
