#include "RouletteScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "InterfaceColorConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RouletteScreen.hpp"
#include "RouletteScreenFixtures.hpp"
#include "RouletteScreenParts.hpp"
#include "RouletteScreenTestConstants.hpp"
#include "ScreenTestConstants.hpp"
#include "Wallet.hpp"

using rtype::client::MotionPreference;
using rtype::client::PLAYER_COLORS;
using rtype::client::RouletteScreen;
using rtype::client::RouletteScreenParts;
using rtype::client::testing::A_ROUND;
using rtype::client::testing::ACE_NAME;
using rtype::client::testing::bettingRoom;
using rtype::client::testing::CHIPS_OF_ACE;
using rtype::client::testing::TABLE_ACE_ACCOUNT;
using rtype::client::testing::TABLE_ACE_TOKEN;

RouletteScreenFixture::RouletteScreenFixture() {
  wallet_.balances =
      rtype::network::Wallet{.kopecks = 0, .chips = CHIPS_OF_ACE};
  account_.signInAs(TABLE_ACE_ACCOUNT, std::string{TABLE_ACE_TOKEN});
  account_.nameAccount(std::string{ACE_NAME});
  casino_.roomState = bettingRoom(A_ROUND);
  screen_ = std::make_unique<RouletteScreen>(
      RouletteScreenParts{.navigator = navigator_,
                          .sounds = sounds_,
                          .audio = audio_,
                          .casino = casino_,
                          .memory = memory_,
                          .wallet = wallet_,
                          .account = account_,
                          .playfieldPointAt = samePlayfieldPoint,
                          .palette = PLAYER_COLORS,
                          .motion = MotionPreference::Full},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
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

RecordingCasinoChannel& RouletteScreenFixture::casino() { return casino_; }

FakeWallet& RouletteScreenFixture::wallet() { return wallet_; }

const RecordingNavigator& RouletteScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& RouletteScreenFixture::canvas() const {
  return canvas_;
}
