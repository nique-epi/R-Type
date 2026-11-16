#include "CasinoEntranceScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CasinoEntranceScreen.hpp"
#include "CasinoEntranceScreenParts.hpp"
#include "CasinoEntranceScreenTestConstants.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingCasinoChannel.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenTestConstants.hpp"
#include "TimeConstants.hpp"
#include "Wallet.hpp"

using rtype::client::CasinoEntranceScreen;
using rtype::client::CasinoEntranceScreenParts;
using rtype::client::MotionPreference;
using rtype::client::testing::CHIPS_HELD;
using rtype::client::testing::DOORS_OPENING_TIME;
using rtype::client::testing::KOPECKS_HELD;

CasinoEntranceScreenFixture::CasinoEntranceScreenFixture() {
  wallet_.balances =
      rtype::network::Wallet{.kopecks = KOPECKS_HELD, .chips = CHIPS_HELD};
  reopenWith(MotionPreference::Full);
}

void CasinoEntranceScreenFixture::reopenWith(MotionPreference motion) {
  screen_ = std::make_unique<CasinoEntranceScreen>(
      CasinoEntranceScreenParts{.navigator = navigator_,
                                .sounds = sounds_,
                                .casino = casino_,
                                .wallet = wallet_,
                                .playfieldPointAt = samePlayfieldPoint,
                                .motion = motion},
      std::make_unique<RecordingInterfaceCanvas>(canvas_));
}

void CasinoEntranceScreenFixture::press(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::hold(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::release(sf::Keyboard::Key key) {
  screen_->handleEvent(keyRelease(key));
  screen_->update(ONE_FRAME);
}

void CasinoEntranceScreenFixture::wait(rtype::engine::Duration elapsed) {
  screen_->update(elapsed);
}

void CasinoEntranceScreenFixture::enterTheHall() {
  press(sf::Keyboard::Key::Enter);
  wait(DOORS_OPENING_TIME);
}

const RecordingNavigator& CasinoEntranceScreenFixture::navigator() const {
  return navigator_;
}

const RecordingCasinoChannel& CasinoEntranceScreenFixture::casino() const {
  return casino_;
}

const InterfaceCanvasRecord& CasinoEntranceScreenFixture::canvas() const {
  return canvas_;
}
