#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include "BoundAction.hpp"
#include "FakeSizableWindow.hpp"
#include "InterfaceColors.hpp"
#include "KeyNames.hpp"
#include "KeyPress.hpp"
#include "OptionsScreen.hpp"
#include "PixelSize.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

using rtype::client::BoundAction;
using rtype::client::DIM_LAYER_COLOR;
using rtype::client::OptionsScreen;
using rtype::client::PixelSize;
using rtype::client::PlayerSettings;
using rtype::client::PRESS_DURATION;
using rtype::client::regularTextWidth;
using Key = sf::Keyboard::Key;
using Scan = sf::Keyboard::Scan;

namespace {

/** @brief A 1920 x 1080 desktop: 960, 1280 and 1600 wide fit, 1600 x 900
 * is the window. */
constexpr PixelSize DESKTOP{.width = 1920, .height = 1080};
constexpr std::size_t LARGEST_FIT = 2;

/** @brief The picture the team chose: SAVED at (270, 36); the description
 * from x 112 to the safe margin at 456; the open list of sizes from x 332;
 * the label of the CONTROLS tab at (158, 55). */
constexpr sf::Vector2i SAVED_ANCHOR(270, 36);
constexpr int DESCRIPTION_LEFT = 112;
constexpr int DESCRIPTION_RIGHT = 456;
constexpr int OPEN_LIST_LEFT = 332;
constexpr sf::Vector2i CONTROLS_TAB(158, 55);
constexpr int MUSIC_DEFAULT = 70;
constexpr int MUSIC_ONE_STEP_UP = 75;
constexpr int QUIET_MUSIC = 10;
constexpr std::size_t ROWS_IN_THE_LONGEST_TAB = 8;
constexpr int TABS = 4;
constexpr std::chrono::seconds LONG_AFTER_A_FAILED_SAVE(10);
constexpr sf::Vector2i ALONG_THE_TRACK(80, 0);

/** @brief Names a key the way the settings file does, as the options would
 * on a US keyboard. */
std::string nameInTheFile(sf::Keyboard::Scancode key) {
  return std::string(rtype::client::keyName(key).value_or("?"));
}

class OptionsScreenTest : public ::testing::Test {
 protected:
  explicit OptionsScreenTest(
      OptionsScreen::Place place = OptionsScreen::Place::OverTitle)
      : screen_(navigator_, place, settings_, storage_, window_, nameInTheFile,
                std::make_unique<RecordingInterfaceCanvas>(canvas_),
                samePlayfieldPoint) {}

  void send(const sf::Event& event) { screen_.handleEvent(event); }

  /** @brief Presses and releases a key, then lets a frame pass that is as
   * long as a press. */
  void tap(Key key, sf::Keyboard::Scancode scancode = Scan::Unknown) {
    send(keyPress(key, scancode));
    send(keyRelease(key, scancode));
    screen_.update(PRESS_DURATION);
  }

  void openNextTab() { tap(Key::E, Scan::E); }

  [[nodiscard]] const RecordedWriting* writing(const std::string& text) const {
    return writingOf(canvas_, text);
  }

  [[nodiscard]] const RecordedWriting* description() const {
    for (const RecordedWriting& written : canvas_.writings) {
      if (written.anchor.x == DESCRIPTION_LEFT &&
          written.text.starts_with("> ")) {
        return &written;
      }
    }
    return nullptr;
  }

  PlayerSettings& settings() { return settings_; }
  RecordingSettingsStorage& storage() { return storage_; }
  FakeSizableWindow& window() { return window_; }
  [[nodiscard]] const RecordingNavigator& navigator() const {
    return navigator_;
  }
  [[nodiscard]] const InterfaceCanvasRecord& canvas() const { return canvas_; }
  OptionsScreen& screen() { return screen_; }

 private:
  PlayerSettings settings_;
  RecordingSettingsStorage storage_;
  FakeSizableWindow window_{DESKTOP};
  RecordingNavigator navigator_;
  InterfaceCanvasRecord canvas_;
  OptionsScreen screen_;
};

class GameOptionsScreenTest : public OptionsScreenTest {
 protected:
  GameOptionsScreenTest() : OptionsScreenTest(OptionsScreen::Place::OverGame) {}
};

}  // namespace

/**
 * Given the options just opened over the title
 * When they are painted
 * Then the SOUND tab shows its four rows
 */
TEST_F(OptionsScreenTest, TheOptionsOpenOnTheSoundTab) {
  EXPECT_NE(writing("MUSIC"), nullptr);
  EXPECT_NE(writing("EFFECTS"), nullptr);
  EXPECT_NE(writing("MUTE ALL"), nullptr);
  EXPECT_NE(writing("DEFAULT SOUND"), nullptr);
}

/**
 * Given the SOUND tab
 * When the key at the place of E is tapped
 * Then the CONTROLS tab shows its rows
 */
TEST_F(OptionsScreenTest, TheNextTabKeyOpensControls) {
  openNextTab();
  EXPECT_NE(writing("MOVE UP"), nullptr);
}

/**
 * Given the SOUND tab
 * When the label of the CONTROLS tab is clicked
 * Then the CONTROLS tab shows its rows
 */
TEST_F(OptionsScreenTest, AClickOnATabOpensIt) {
  send(leftPress(CONTROLS_TAB * INTERFACE_PIXEL));
  screen().update(PRESS_DURATION);
  EXPECT_NE(writing("MOVE UP"), nullptr);
}

/**
 * Given MUSIC at 70, selected
 * When Right is tapped
 * Then the music is at 75 and those settings are saved once
 */
TEST_F(OptionsScreenTest, RightOnMusicRaisesItAndSaves) {
  tap(Key::Right);
  EXPECT_EQ(settings().musicVolume, MUSIC_ONE_STEP_UP);
  ASSERT_EQ(storage().saves().size(), 1U);
  EXPECT_EQ(storage().saves().front().musicVolume, MUSIC_ONE_STEP_UP);
}

/**
 * Given a change just saved
 * When the options are painted
 * Then SAVED is stamped at the end of the rule
 */
TEST_F(OptionsScreenTest, ASaveIsStampedSaved) {
  tap(Key::Right);
  const RecordedWriting* stamp = writing("SAVED");
  ASSERT_NE(stamp, nullptr);
  EXPECT_EQ(stamp->anchor, SAVED_ANCHOR);
}

/**
 * Given SAVED just stamped
 * When a second goes by
 * Then the stamp is gone
 */
TEST_F(OptionsScreenTest, SavedFadesAfterASecond) {
  tap(Key::Right);
  screen().update(std::chrono::seconds(1));
  EXPECT_EQ(writing("SAVED"), nullptr);
}

/**
 * Given the options just opened, nothing changed
 * When frames go by
 * Then nothing is saved: the file is written only after a change
 */
TEST_F(OptionsScreenTest, AFrameWithoutAChangeSavesNothing) {
  screen().update(PRESS_DURATION);
  screen().update(PRESS_DURATION);
  EXPECT_TRUE(storage().saves().empty());
}

/**
 * Given saves refused, as on a full disk
 * When the music is raised, then ten seconds go by
 * Then NOT SAVED stays on the screen
 */
TEST_F(OptionsScreenTest, AFailedSaveStaysStampedNotSaved) {
  storage().refuseSaves(true);
  tap(Key::Right);
  screen().update(LONG_AFTER_A_FAILED_SAVE);
  EXPECT_NE(writing("NOT SAVED"), nullptr);
}

/**
 * Given the thumb of MUSIC pressed at the left of its track
 * When the pointer drags it right and a frame passes, the button still held
 * Then the volume dragged to is already saved, so closing the window then
 * loses nothing
 */
TEST_F(OptionsScreenTest, ADraggedVolumeIsSavedBeforeTheRelease) {
  const sf::Vector2i trackStart(281, 78);
  send(leftPress(trackStart * INTERFACE_PIXEL));
  send(pointerMove((trackStart + ALONG_THE_TRACK) * INTERFACE_PIXEL));
  screen().update(PRESS_DURATION);
  ASSERT_FALSE(storage().saves().empty());
  EXPECT_EQ(storage().saves().back().musicVolume, settings().musicVolume);
  EXPECT_NE(settings().musicVolume, MUSIC_DEFAULT);
}

/**
 * Given the sound muted
 * When MUSIC is selected
 * Then its description says why its slider is dimmed
 */
TEST_F(OptionsScreenTest, AMutedVolumeSaysWhy) {
  settings().muted = true;
  screen().update(PRESS_DURATION);
  ASSERT_NE(description(), nullptr);
  EXPECT_EQ(description()->text, "> SOUND MUTED: SEE MUTE ALL");
}

/**
 * Given the music at 10
 * When DEFAULT SOUND is activated
 * Then a question is asked, and the volume is still 10
 */
TEST_F(OptionsScreenTest, DefaultSoundAsksFirst) {
  settings().musicVolume = QUIET_MUSIC;
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Enter);
  EXPECT_NE(writing("PUT BACK THE DEFAULT SOUND?"), nullptr);
  EXPECT_EQ(settings().musicVolume, QUIET_MUSIC);
}

/**
 * Given the question of DEFAULT SOUND
 * When YES is chosen
 * Then the music is back at 70, saved, and the question is gone
 */
TEST_F(OptionsScreenTest, YesPutsTheVolumesBack) {
  settings().musicVolume = QUIET_MUSIC;
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Enter);
  tap(Key::Right);
  tap(Key::Enter);
  EXPECT_EQ(settings().musicVolume, MUSIC_DEFAULT);
  ASSERT_FALSE(storage().saves().empty());
  EXPECT_EQ(storage().saves().back().musicVolume, MUSIC_DEFAULT);
  EXPECT_EQ(writing("PUT BACK THE DEFAULT SOUND?"), nullptr);
}

/**
 * Given the question of DEFAULT SOUND
 * When Escape is tapped
 * Then the volumes are kept, the question is gone and the options stay open
 */
TEST_F(OptionsScreenTest, EscapeOnTheQuestionKeepsTheVolumes) {
  settings().musicVolume = QUIET_MUSIC;
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Enter);
  tap(Key::Escape);
  EXPECT_EQ(settings().musicVolume, QUIET_MUSIC);
  EXPECT_EQ(writing("PUT BACK THE DEFAULT SOUND?"), nullptr);
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given MOVE UP selected in CONTROLS
 * When Enter is tapped, then W
 * Then W moves up, and the settings are saved
 */
TEST_F(OptionsScreenTest, AKeyIsChangedByPressingTheNewOne) {
  openNextTab();
  tap(Key::Enter);
  ASSERT_NE(description(), nullptr);
  EXPECT_EQ(description()->text, "> PRESS THE NEW KEY. ESC CANCELS");
  tap(Key::W, Scan::W);
  EXPECT_EQ(settings().keys.keyOf(BoundAction::MoveUp), Scan::W);
  EXPECT_FALSE(storage().saves().empty());
}

/**
 * Given MOVE UP waiting for a key
 * When the Command key is pressed
 * Then it goes on waiting, and its description says the key cannot be bound
 */
TEST_F(OptionsScreenTest, ARefusedKeySaysItCannotBeBound) {
  openNextTab();
  tap(Key::Enter);
  tap(Key::LSystem, Scan::LSystem);
  ASSERT_NE(description(), nullptr);
  EXPECT_EQ(description()->text, "> THIS KEY CANNOT BE BOUND");
  EXPECT_EQ(settings().keys.keyOf(BoundAction::MoveUp), Scan::Up);
}

/**
 * Given MOVE UP waiting for a key
 * When Escape is tapped
 * Then the key is kept and the options stay open
 */
TEST_F(OptionsScreenTest, EscapeWhileWaitingKeepsTheKey) {
  openNextTab();
  tap(Key::Enter);
  tap(Key::Escape, Scan::Escape);
  EXPECT_EQ(settings().keys.keyOf(BoundAction::MoveUp), Scan::Up);
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given MOVE UP moved onto space, the key of SHOOT
 * When MOVE UP is selected
 * Then its description names SHOOT, and a flag is drawn
 */
TEST_F(OptionsScreenTest, AKeySharedByTwoActionsIsSignaled) {
  openNextTab();
  tap(Key::Enter);
  tap(Key::Space, Scan::Space);
  ASSERT_NE(description(), nullptr);
  EXPECT_EQ(description()->text, "> SAME KEY AS SHOOT: CHANGE ONE");
  EXPECT_NE(writing("!"), nullptr);
}

/**
 * Given the DISPLAY tab, the window at 1600 x 900
 * When Enter opens the list, Up then Enter take 1280 x 720
 * Then the window is 1280 x 720, and that size is saved
 */
TEST_F(OptionsScreenTest, ASizeTakenResizesTheWindowAndIsSaved) {
  openNextTab();
  openNextTab();
  tap(Key::Enter);
  tap(Key::Up);
  tap(Key::Enter);
  EXPECT_EQ(window().windowSizeSelection().selectedIndex(), 1U);
  ASSERT_FALSE(storage().saves().empty());
  EXPECT_EQ(storage().saves().back().windowSize,
            (PixelSize{.width = 1280, .height = 720}));
}

/**
 * Given the DISPLAY tab
 * When the list of sizes opens
 * Then everything it adds to the picture lies right of x 332, clear of the
 * chart and its legend on the left
 */
TEST_F(OptionsScreenTest, TheOpenListLeavesThePreviewUncovered) {
  openNextTab();
  openNextTab();
  const auto closed = canvas().fills;
  tap(Key::Enter);
  for (const RecordedFill& fill : canvas().fills) {
    if (std::ranges::find(closed, fill) == closed.end()) {
      EXPECT_GE(fill.area.position.x, OPEN_LIST_LEFT);
    }
  }
}

/**
 * Given the DISPLAY tab, the window at the largest size that fits
 * When End is tapped in the open list
 * Then the highlight stays on that size: a size too large cannot be reached
 */
TEST_F(OptionsScreenTest, ASizeTooLargeCannotBeReached) {
  openNextTab();
  openNextTab();
  tap(Key::Enter);
  tap(Key::End);
  tap(Key::Enter);
  EXPECT_EQ(window().windowSizeSelection().selectedIndex(), LARGEST_FIT);
}

/**
 * Given every row of every tab
 * When each is selected in turn
 * Then its description fits between BACK and the safe margin
 */
TEST_F(OptionsScreenTest, EveryDescriptionFitsBesideBack) {
  for (int tab = 0; tab < TABS; ++tab) {
    for (std::size_t row = 0; row < ROWS_IN_THE_LONGEST_TAB; ++row) {
      const RecordedWriting* line = description();
      if (line != nullptr) {
        EXPECT_LE(DESCRIPTION_LEFT + regularTextWidth(line->text),
                  DESCRIPTION_RIGHT)
            << line->text;
      }
      tap(Key::Down);
    }
    openNextTab();
  }
}

/**
 * Given the options
 * When BACK is activated by Escape
 * Then they close
 */
TEST_F(OptionsScreenTest, EscapeClosesTheOptions) {
  tap(Key::Escape, Scan::Escape);
  EXPECT_THAT(navigator().calls(),
              ::testing::ElementsAre(NavigatorCall{
                  .request = NavigatorRequest::Pop, .screen = std::nullopt}));
}

/**
 * Given the options over the title
 * When the stack asks whether they hide the screens below
 * Then they do
 */
TEST_F(OptionsScreenTest, OverTheTitleTheOptionsHideIt) {
  EXPECT_TRUE(screen().hidesScreensBelow());
}

/**
 * Given the options over a game
 * When they are painted
 * Then the game shows, dimmed first, and THE GAME GOES ON is written
 */
TEST_F(GameOptionsScreenTest, OverAGameTheOptionsDimItAndSaySo) {
  EXPECT_FALSE(screen().hidesScreensBelow());
  ASSERT_FALSE(canvas().fills.empty());
  EXPECT_EQ(canvas().fills.front().color, DIM_LAYER_COLOR);
  EXPECT_NE(writing("THE GAME GOES ON"), nullptr);
}
