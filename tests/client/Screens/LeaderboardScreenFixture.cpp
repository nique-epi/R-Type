#include "LeaderboardScreenFixture.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <utility>
#include "LeaderboardFilterChoice.hpp"
#include "LeaderboardPage.hpp"
#include "LeaderboardScreen.hpp"
#include "LeaderboardScreenParts.hpp"
#include "LeaderboardTab.hpp"
#include "MotionPreference.hpp"
#include "OptionsScreenFixtures.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingLeaderboardSource.hpp"
#include "RecordingNavigator.hpp"
#include "ScreenInputs.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::LeaderboardFilterChoice;
using rtype::client::LeaderboardPage;
using rtype::client::LeaderboardScreen;
using rtype::client::LeaderboardScreenParts;
using rtype::client::LeaderboardTab;
using rtype::client::MotionPreference;

LeaderboardScreenFixture::LeaderboardScreenFixture(MotionPreference motion)
    : screen_(
          LeaderboardScreenParts{
              .navigator = navigator_,
              .sounds = sounds_,
              .tabs = {LeaderboardTab{.label = "SERVER", .source = &source_},
                       LeaderboardTab{.label = "GLOBAL"}},
              .choice = LeaderboardFilterChoice{.difficulty = 1, .pilots = 1},
              .playfieldPointAt = samePlayfieldPoint,
              .keyName = nameInTheFile,
              .motion = motion},
          std::make_unique<RecordingInterfaceCanvas>(canvas_)) {}

void LeaderboardScreenFixture::tap(sf::Keyboard::Key key) {
  tapKey(screen_, key);
}

void LeaderboardScreenFixture::show(LeaderboardPage page) {
  source_.setPage(std::move(page));
  screen_.update(ONE_FRAME);
}

const RecordingNavigator& LeaderboardScreenFixture::navigator() const {
  return navigator_;
}

RecordingLeaderboardSource& LeaderboardScreenFixture::source() {
  return source_;
}

const InterfaceCanvasRecord& LeaderboardScreenFixture::canvas() const {
  return canvas_;
}

LeaderboardScreen& LeaderboardScreenFixture::screen() { return screen_; }

StillLeaderboardFixture::StillLeaderboardFixture()
    : LeaderboardScreenFixture(MotionPreference::Reduced) {}
