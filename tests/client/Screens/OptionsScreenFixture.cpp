#include "OptionsScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "FakeSizableWindow.hpp"
#include "KeyPress.hpp"
#include "OptionsPlace.hpp"
#include "OptionsScreen.hpp"
#include "OptionsScreenFixtures.hpp"
#include "OptionsScreenParts.hpp"
#include "OptionsScreenTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "RecordingSoundPlayer.hpp"
#include "WidgetConstants.hpp"

using rtype::client::OptionsPlace;
using rtype::client::OptionsScreen;
using rtype::client::OptionsScreenParts;
using rtype::client::PlayerSettings;

OptionsScreenFixture::OptionsScreenFixture(OptionsPlace place)
    : window_(rtype::client::testing::DESKTOP),
      screen_(OptionsScreenParts{.navigator = navigator_,
                                 .sounds = sounds_,
                                 .place = place,
                                 .settings = settings_,
                                 .storage = storage_,
                                 .window = window_,
                                 .keyName = nameInTheFile,
                                 .playfieldPointAt = samePlayfieldPoint},
              std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

void OptionsScreenFixture::tap(sf::Keyboard::Key key,
                               sf::Keyboard::Scancode scancode) {
  screen_.handleEvent(keyPress(key, scancode));
  screen_.handleEvent(keyRelease(key, scancode));
  screen_.update(rtype::client::PRESS_DURATION);
}

void OptionsScreenFixture::openNextTab() {
  tap(sf::Keyboard::Key::E, sf::Keyboard::Scan::E);
}

void OptionsScreenFixture::takeBorderless() {
  tap(sf::Keyboard::Key::Enter);
  tap(sf::Keyboard::Key::Down);
  tap(sf::Keyboard::Key::Enter);
}

PlayerSettings& OptionsScreenFixture::settings() { return settings_; }

RecordingSettingsStorage& OptionsScreenFixture::storage() { return storage_; }

FakeSizableWindow& OptionsScreenFixture::window() { return window_; }

const RecordingNavigator& OptionsScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& OptionsScreenFixture::canvas() const {
  return canvas_;
}

OptionsScreen& OptionsScreenFixture::screen() { return screen_; }

const RecordingSoundPlayer& OptionsScreenFixture::sounds() const {
  return sounds_;
}
