#include "TitleInTheStackFixture.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include "IScreen.hpp"
#include "MotionPreference.hpp"
#include "PointerEvents.hpp"
#include "QuitConfirmationScreen.hpp"
#include "QuitConfirmationScreenParts.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "TitleScreen.hpp"
#include "TitleScreenParts.hpp"
#include "TitleScreenTestConstants.hpp"
#include "WidgetConstants.hpp"

using rtype::client::IScreen;
using rtype::client::MotionPreference;
using rtype::client::PRESS_DURATION;
using rtype::client::QuitConfirmationScreen;
using rtype::client::QuitConfirmationScreenParts;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using rtype::client::TitleScreen;
using rtype::client::TitleScreenParts;

TitleInTheStackFixture::TitleInTheStackFixture()
    : screens_([this](ScreenId screen) { return build(screen); }) {
  screens_.replaceAll(ScreenId::Title);
  screens_.applyPendingChanges();
}

const RecordingNavigator& TitleInTheStackFixture::titleNavigator() const {
  return titleNavigator_;
}

const InterfaceCanvasRecord& TitleInTheStackFixture::titleCanvas() const {
  return titleCanvas_;
}

ScreenStack& TitleInTheStackFixture::screens() { return screens_; }

void TitleInTheStackFixture::open(ScreenId screen) {
  screens_.push(screen);
  screens_.applyPendingChanges();
}

void TitleInTheStackFixture::closeTheScreenOnTop() {
  screens_.pop();
  screens_.applyPendingChanges();
}

void TitleInTheStackFixture::click(sf::Vector2i position) {
  screens_.handleEvent(leftPress(position));
  screens_.handleEvent(leftRelease(position));
  screens_.update(PRESS_DURATION);
}

std::unique_ptr<IScreen> TitleInTheStackFixture::build(ScreenId screen) {
  if (screen == ScreenId::Title) {
    return std::make_unique<TitleScreen>(
        TitleScreenParts{.navigator = titleNavigator_,
                         .sounds = sounds_,
                         .playfieldPointAt = samePlayfieldPoint,
                         .motion = MotionPreference::Full,
                         .seed = rtype::client::testing::TITLE_SEED},
        std::make_unique<RecordingInterfaceCanvas>(titleCanvas_));
  }
  if (screen == ScreenId::QuitConfirmation) {
    return std::make_unique<QuitConfirmationScreen>(
        QuitConfirmationScreenParts{.navigator = dialogNavigator_,
                                    .sounds = sounds_,
                                    .playfieldPointAt = samePlayfieldPoint},
        std::make_unique<RecordingInterfaceCanvas>(dialogCanvas_));
  }
  auto options = std::make_unique<RecordingScreen>(screen, journal_);
  options->setHidesScreensBelow(true);
  return options;
}
