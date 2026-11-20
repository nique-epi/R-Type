#include "CashierScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CashierRoom.hpp"
#include "CashierScreen.hpp"
#include "CashierScreenParts.hpp"
#include "CashierScreenTestConstants.hpp"
#include "FakeWallet.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"
#include "ShipLocker.hpp"
#include "Wallet.hpp"

using rtype::client::CashierRoom;
using rtype::client::CashierScreen;
using rtype::client::CashierScreenParts;
using rtype::client::MotionPreference;
using rtype::client::ShipLocker;
using rtype::client::testing::CHIPS_HELD;
using rtype::client::testing::KOPECKS_HELD;

CashierScreenFixture::CashierScreenFixture() {
  wallet_.balances =
      rtype::network::Wallet{.kopecks = KOPECKS_HELD, .chips = CHIPS_HELD};
  wallet_.looks = ShipLocker{};
  reopenWith(MotionPreference::Full);
}

void CashierScreenFixture::reopenWith(MotionPreference motion) {
  screen_ = std::make_unique<CashierScreen>(
      CashierScreenParts{.navigator = navigator_,
                         .sounds = sounds_,
                         .audio = audio_,
                         .wallet = wallet_,
                         .playfieldPointAt = samePlayfieldPoint,
                         .room = CashierRoom{},
                         .motion = motion},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void CashierScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CashierScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void CashierScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CashierScreenFixture::clickAt(sf::Vector2i interfacePixel) {
  const sf::Vector2i windowPixel = interfacePixel * INTERFACE_PIXEL;
  screen_->handleEvent(leftPress(windowPixel));
  screen_->handleEvent(leftRelease(windowPixel));
  screen_->update(ONE_FRAME);
}

void CashierScreenFixture::wait() { screen_->update(ONE_FRAME); }

void CashierScreenFixture::waitFor(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

FakeWallet& CashierScreenFixture::wallet() { return wallet_; }

const RecordingNavigator& CashierScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& CashierScreenFixture::canvas() const {
  return canvas_;
}
