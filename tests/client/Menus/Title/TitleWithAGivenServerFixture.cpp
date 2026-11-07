#include "TitleWithAGivenServerFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "KeyPress.hpp"
#include "MotionPreference.hpp"
#include "PlayStart.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "TitleScreenParts.hpp"
#include "TitleScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::MotionPreference;
using rtype::client::PlayStart;
using rtype::client::PRESS_DURATION;
using rtype::client::TitleScreenParts;

TitleWithAGivenServerFixture::TitleWithAGivenServerFixture()
    : title_(TitleScreenParts{.navigator = navigator_,
                              .sounds = sounds_,
                              .settings = settings_,
                              .playfieldPointAt = samePlayfieldPoint,
                              .motion = MotionPreference::Full,
                              .seed = rtype::client::testing::TITLE_SEED,
                              .playStart = PlayStart::GivenServer},
             std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

const RecordingNavigator& TitleWithAGivenServerFixture::navigator() const {
  return navigator_;
}

void TitleWithAGivenServerFixture::choose(sf::Keyboard::Key key) {
  title_.handleEvent(keyPress(key));
  title_.handleEvent(keyRelease(key));
  title_.update(PRESS_DURATION);
}
