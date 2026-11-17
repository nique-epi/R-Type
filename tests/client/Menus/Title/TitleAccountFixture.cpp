#include "TitleAccountFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include "FakeAccountSession.hpp"
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingDeviceLinkFlow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "TitleAccountTestConstants.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"
#include "TitleScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::TitleScreen;
using rtype::client::TitleScreenParts;
using rtype::client::testing::TITLE_ACCOUNT_ID;
using rtype::client::testing::TITLE_ACCOUNT_NICKNAME;
using rtype::client::testing::TITLE_ACCOUNT_TOKEN;

TitleAccountFixture::TitleAccountFixture()
    : title_(std::make_unique<TitleScreen>(
          TitleScreenParts{.navigator = navigator_,
                           .sounds = sounds_,
                           .settings = settings_,
                           .wallet = wallet_,
                           .account = account_,
                           .deviceLinks = deviceLinks_,
                           .signInReturn = signInReturn_,
                           .playfieldPointAt = samePlayfieldPoint,
                           .motion = MotionPreference::Full,
                           .seed = rtype::client::testing::TITLE_SEED},
          std::make_unique<RecordingInterfaceCanvas>(canvas_))) {}

const RecordingNavigator& TitleAccountFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& TitleAccountFixture::canvas() const {
  return canvas_;
}

FakeAccountSession& TitleAccountFixture::account() { return account_; }

const RecordingDeviceLinkFlow& TitleAccountFixture::deviceLinks() const {
  return deviceLinks_;
}

void TitleAccountFixture::signInAsAce() {
  account_.nameAccount(std::string{TITLE_ACCOUNT_NICKNAME});
  account_.signInAs(TITLE_ACCOUNT_ID, std::string{TITLE_ACCOUNT_TOKEN});
  title_->update(PRESS_DURATION);
}

void TitleAccountFixture::chooseTheAccountBar() {
  title_->handleEvent(keyPress(sf::Keyboard::Key::Up));
  title_->handleEvent(keyRelease(sf::Keyboard::Key::Up));
  title_->handleEvent(keyPress(sf::Keyboard::Key::Up));
  title_->handleEvent(keyRelease(sf::Keyboard::Key::Up));
  title_->handleEvent(keyPress(sf::Keyboard::Key::Enter));
  title_->handleEvent(keyRelease(sf::Keyboard::Key::Enter));
  title_->update(PRESS_DURATION);
}
