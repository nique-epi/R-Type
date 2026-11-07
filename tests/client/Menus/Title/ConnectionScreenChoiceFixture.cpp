#include "ConnectionScreenChoiceFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include "ConnectionScreen.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionStart.hpp"
#include "Endpoint.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenInputs.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionScreen;
using rtype::client::ConnectionScreenParts;
using rtype::client::ConnectionStart;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::MAP_ADDRESS;
using rtype::client::testing::MAP_PORT;
using rtype::network::Endpoint;

ConnectionScreenChoiceFixture::ConnectionScreenChoiceFixture()
    : choice_(launchChoice()) {
  reopen();
}

void ConnectionScreenChoiceFixture::connectAs(std::u32string_view nickname) {
  for (const sf::Keyboard::Key move :
       {sf::Keyboard::Key::Down, sf::Keyboard::Key::Down}) {
    tapKey(*screen_, move);
  }
  for (std::size_t erased = 0; erased < LAUNCH_NICKNAME.size(); ++erased) {
    screen_->handleEvent(keyPress(sf::Keyboard::Key::Backspace));
  }
  for (const char32_t character : nickname) {
    screen_->handleEvent(typedCharacter(character));
  }
  chooseEnter();
}

void ConnectionScreenChoiceFixture::reopen() {
  openWith(ConnectionStart::OnConfirm);
}

void ConnectionScreenChoiceFixture::reopenConnectingAtOnce() {
  openWith(ConnectionStart::AtOnce);
}

void ConnectionScreenChoiceFixture::reopenForARetry() {
  openWith(ConnectionStart::LastAttemptOnConfirm);
}

void ConnectionScreenChoiceFixture::chooseOnTheMap() {
  choice_.lastServer =
      Endpoint{.address = std::string(MAP_ADDRESS), .port = MAP_PORT};
}

void ConnectionScreenChoiceFixture::openWith(ConnectionStart start) {
  screen_.reset();
  screen_ = std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .connection = connection_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .choice = choice_,
                            .start = start},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void ConnectionScreenChoiceFixture::chooseEnter() {
  tapKey(*screen_, sf::Keyboard::Key::Enter);
  screen_->update(PRESS_DURATION);
}

const RecordingServerConnection& ConnectionScreenChoiceFixture::connection()
    const {
  return connection_;
}
