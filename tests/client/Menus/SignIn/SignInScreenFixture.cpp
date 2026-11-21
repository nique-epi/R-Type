#include "SignInScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>
#include <vector>
#include "FakeAccountSession.hpp"
#include "KeyPress.hpp"
#include "PointerEvents.hpp"
#include "RecordingDeviceLinkFlow.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "SignInReturn.hpp"
#include "SignInScreen.hpp"
#include "SignInScreenParts.hpp"
#include "SignInTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::PRESS_DURATION;
using rtype::client::SignInReturn;
using rtype::client::SignInScreen;
using rtype::client::SignInScreenParts;
using rtype::client::testing::SIGN_IN_NOW;

SignInScreenFixture::SignInScreenFixture()
    : screen_(std::make_unique<SignInScreen>(
          SignInScreenParts{.navigator = navigator_,
                            .deviceLinks = deviceLinks_,
                            .account = account_,
                            .signInReturn = signInReturn_,
                            .sounds = sounds_,
                            .playfieldPointAt = samePlayfieldPoint,
                            .openPage =
                                [this](const std::string& page) {
                                  openedPages_.push_back(page);
                                  return true;
                                },
                            .now = [] { return SIGN_IN_NOW; }},
          std::make_unique<RecordingInterfaceCanvas>(canvas_))) {}

const RecordingNavigator& SignInScreenFixture::navigator() const {
  return navigator_;
}

RecordingDeviceLinkFlow& SignInScreenFixture::deviceLinks() {
  return deviceLinks_;
}

FakeAccountSession& SignInScreenFixture::account() { return account_; }

SignInReturn& SignInScreenFixture::signInReturn() { return signInReturn_; }

const std::vector<std::string>& SignInScreenFixture::openedPages() const {
  return openedPages_;
}

const InterfaceCanvasRecord& SignInScreenFixture::painting() const {
  return canvas_;
}

void SignInScreenFixture::frame() { screen_->update(PRESS_DURATION); }

void SignInScreenFixture::choose(sf::Keyboard::Key key) {
  screen_->handleEvent(keyPress(key));
  screen_->handleEvent(keyRelease(key));
  frame();
}
