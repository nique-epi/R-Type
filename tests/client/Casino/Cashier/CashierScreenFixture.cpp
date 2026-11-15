#include "CashierScreenFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "CashierScreen.hpp"
#include "CashierScreenParts.hpp"
#include "CashierScreenTestConstants.hpp"
#include "CashierWay.hpp"
#include "KeyPress.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::CashierScreen;
using rtype::client::CashierScreenParts;
using rtype::client::CashierWay;
using rtype::client::PlayerSettings;
using rtype::client::testing::CHIPS_HELD;
using rtype::client::testing::KOPECKS_HELD;

CashierScreenFixture::CashierScreenFixture() : service_(settings_, storage_) {
  settings_.kopecks = KOPECKS_HELD;
  settings_.chips = CHIPS_HELD;
  reopenWith(CashierWay::BuyChips);
}

void CashierScreenFixture::reopenWith(CashierWay way) {
  screen_ = std::make_unique<CashierScreen>(
      CashierScreenParts{.navigator = navigator_,
                         .sounds = sounds_,
                         .settings = service_,
                         .playfieldPointAt = samePlayfieldPoint,
                         .way = way},
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

const PlayerSettings& CashierScreenFixture::settings() const {
  return settings_;
}

const RecordingSettingsStorage& CashierScreenFixture::storage() const {
  return storage_;
}

const RecordingNavigator& CashierScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& CashierScreenFixture::canvas() const {
  return canvas_;
}
