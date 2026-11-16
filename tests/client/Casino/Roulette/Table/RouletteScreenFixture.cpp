#include "RouletteScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include "ConnectionStatus.hpp"
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "LobbyState.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RouletteRound.hpp"
#include "RouletteScreen.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenParts.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "Wallet.hpp"

using rtype::client::ConnectionStatus;
using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::RouletteScreen;
using rtype::client::RouletteScreenParts;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::ACE_NAME;
using rtype::client::testing::ACE_NUMBER;
using rtype::client::testing::bettingRound;
using rtype::client::testing::CHIPS_OF_ACE;
using rtype::client::testing::LUNA_NAME;
using rtype::client::testing::LUNA_NUMBER;
using rtype::network::LobbyPlayer;

RouletteScreenFixture::RouletteScreenFixture() {
  wallet_.balances =
      rtype::network::Wallet{.kopecks = 0, .chips = CHIPS_OF_ACE};
  connection_.setStatus(ConnectionStatus{.playerNumber = ACE_NUMBER});
  waitingRoom_.setLobbyPlayers(
      {LobbyPlayer{.playerNumber = ACE_NUMBER,
                   .nickname = std::string(ACE_NAME)},
       LobbyPlayer{.playerNumber = LUNA_NUMBER,
                   .nickname = std::string(LUNA_NAME)}});
  casino_.setRound(bettingRound(A_ROUND, 0));
  screen_ = std::make_unique<RouletteScreen>(
      RouletteScreenParts{.navigator = navigator_,
                          .sounds = sounds_,
                          .casino = casino_,
                          .memory = memory_,
                          .wallet = wallet_,
                          .connection = connection_,
                          .waitingRoom = waitingRoom_,
                          .playfieldPointAt = samePlayfieldPoint,
                          .palette = PLAYER_COLORS,
                          .motion = MotionPreference::Full},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void RouletteScreenFixture::showRound(
    const rtype::network::RouletteRound& round) {
  casino_.setRound(round);
  screen_->update(ONE_FRAME);
}

void RouletteScreenFixture::holdChips(std::uint32_t chips) {
  wallet_.balances = rtype::network::Wallet{.kopecks = 0, .chips = chips};
  screen_->update(ONE_FRAME);
}

void RouletteScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void RouletteScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void RouletteScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void RouletteScreenFixture::wait(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

void RouletteScreenFixture::clickAt(sf::Vector2i interfacePixel) {
  const sf::Vector2i windowPixel = interfacePixel * INTERFACE_PIXEL;
  screen_->handleEvent(leftPress(windowPixel));
  screen_->handleEvent(leftRelease(windowPixel));
  screen_->update(ONE_FRAME);
}

const RecordingCasinoChannel& RouletteScreenFixture::casino() const {
  return casino_;
}

const RecordingNavigator& RouletteScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& RouletteScreenFixture::canvas() const {
  return canvas_;
}
