#include "ConnectionToMapServerFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include <string_view>
#include "ConnectionRequest.hpp"
#include "ConnectionScreen.hpp"
#include "ConnectionScreenFixtures.hpp"
#include "ConnectionScreenParts.hpp"
#include "ConnectionScreenTestConstants.hpp"
#include "ConnectionStart.hpp"
#include "Endpoint.hpp"
#include "GameRequest.hpp"
#include "GameRules.hpp"
#include "GameRulesTestConstants.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingServerConnection.hpp"
#include "ScreenInputs.hpp"
#include "WidgetConstants.hpp"

using rtype::client::ConnectionScreen;
using rtype::client::ConnectionScreenParts;
using rtype::client::ConnectionStart;
using rtype::client::GameRequest;
using rtype::client::PRESS_DURATION;
using rtype::client::testing::LAUNCH_NICKNAME;
using rtype::client::testing::MAP_ADDRESS;
using rtype::client::testing::MAP_PORT;
using rtype::network::Endpoint;
using rtype::network::GameRules;
using rtype::testing::CHOSEN_DIFFICULTY;
using rtype::testing::CHOSEN_LIVES;

ConnectionToMapServerFixture::ConnectionToMapServerFixture()
    : choice_(launchChoice()) {
  choice_.lastServer =
      Endpoint{.address = std::string(MAP_ADDRESS), .port = MAP_PORT};
  open();
}

void ConnectionToMapServerFixture::chooseANewGame() {
  choice_.lastRequest = GameRequest{
      .game = rtype::network::NEW_GAME_CHOICE,
      .rules = GameRules{.difficulty = std::string(CHOSEN_DIFFICULTY),
                         .lives = CHOSEN_LIVES,
                         .friendlyFire = true}};
  open();
}

void ConnectionToMapServerFixture::typeNickname(std::u32string_view nickname) {
  eraseCharacters(*screen_, LAUNCH_NICKNAME.size());
  typeText(*screen_, nickname);
}

void ConnectionToMapServerFixture::chooseEnter() {
  tapKey(*screen_, sf::Keyboard::Key::Enter);
  screen_->update(PRESS_DURATION);
}

const RecordingServerConnection& ConnectionToMapServerFixture::connection()
    const {
  return connection_;
}

const InterfaceCanvasRecord& ConnectionToMapServerFixture::canvas() const {
  return canvas_;
}

void ConnectionToMapServerFixture::open() {
  screen_.reset();
  screen_ = std::make_unique<ConnectionScreen>(
      ConnectionScreenParts{.navigator = navigator_,
                            .sounds = sounds_,
                            .connection = connection_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .choice = choice_,
                            .start = ConnectionStart::NicknameOnConfirm},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}
