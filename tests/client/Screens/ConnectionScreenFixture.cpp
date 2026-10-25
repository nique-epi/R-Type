#include "ConnectionScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include "ConnectionScreen.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionStatus.hpp"
#include "Endpoint.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingServerConnection.hpp"
#include "RecordingSoundPlayer.hpp"
#include "ScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionScreen;
using rtype::client::ConnectionScreenParts;
using rtype::client::ConnectionStatus;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::LAUNCH_ADDRESS;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::LAUNCH_PORT;
using rtype::network::Endpoint;

ConnectionScreenFixture::ConnectionScreenFixture()
    : screen_(
          ConnectionScreenParts{
              .navigator = navigator_,
              .sounds = sounds_,
              .connection = connection_,
              .playfieldPointAt = samePlayfieldPoint,
              .server = Endpoint{.address = std::string(LAUNCH_ADDRESS),
                                 .port = LAUNCH_PORT},
              .nickname = std::string(LAUNCH_NICKNAME)},
          std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

const RecordingNavigator& ConnectionScreenFixture::navigator() const {
  return navigator_;
}

RecordingServerConnection& ConnectionScreenFixture::connection() {
  return connection_;
}

const InterfaceCanvasRecord& ConnectionScreenFixture::canvas() const {
  return canvas_;
}

ConnectionScreen& ConnectionScreenFixture::screen() { return screen_; }

void ConnectionScreenFixture::tap(sf::Keyboard::Key key) {
  screen_.handleEvent(keyPress(key));
  screen_.handleEvent(keyRelease(key));
}

void ConnectionScreenFixture::choose(sf::Keyboard::Key key) {
  tap(key);
  screen_.update(PRESS_DURATION);
}

void ConnectionScreenFixture::type(std::u32string_view text) {
  for (const char32_t character : text) {
    screen_.handleEvent(typedCharacter(character));
  }
}

void ConnectionScreenFixture::erase(std::size_t characters) {
  for (std::size_t erased = 0; erased < characters; ++erased) {
    screen_.handleEvent(keyPress(sf::Keyboard::Key::Backspace));
  }
}

void ConnectionScreenFixture::show(ConnectionStatus status) {
  connection_.setStatus(status);
  screen_.update(ONE_FRAME);
}

const RecordingSoundPlayer& ConnectionScreenFixture::sounds() const {
  return sounds_;
}
