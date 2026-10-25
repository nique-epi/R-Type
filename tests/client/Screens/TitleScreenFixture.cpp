#include "TitleScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSoundPlayer.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"
#include "TitleScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::TitleScreen;
using rtype::client::TitleScreenParts;

TitleScreenFixture::TitleScreenFixture()
    : title_(TitleScreenParts{.navigator = navigator_,
                              .sounds = sounds_,
                              .playfieldPointAt = samePlayfieldPoint,
                              .motion = MotionPreference::Full,
                              .seed = rtype::client::testing::TITLE_SEED},
             std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

const RecordingNavigator& TitleScreenFixture::navigator() const {
  return navigator_;
}

const InterfaceCanvasRecord& TitleScreenFixture::canvas() const {
  return canvas_;
}

TitleScreen& TitleScreenFixture::title() { return title_; }

void TitleScreenFixture::choose(sf::Keyboard::Key key) {
  title_.handleEvent(keyPress(key));
  title_.handleEvent(keyRelease(key));
  title_.update(PRESS_DURATION);
}

const RecordingSoundPlayer& TitleScreenFixture::sounds() const {
  return sounds_;
}
