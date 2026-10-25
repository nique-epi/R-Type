#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstddef>
#include <optional>
#include "BoundAction.hpp"
#include "FakeSizableWindow.hpp"
#include "GameOptionsScreenFixture.hpp"
#include "InterfaceColorConstants.hpp"
#include "OptionsScreen.hpp"
#include "OptionsScreenFixture.hpp"
#include "OptionsScreenFixtures.hpp"
#include "OptionsScreenTestConstants.hpp"
#include "PlayerSettings.hpp"
#include "PointerEvents.hpp"
#include "RecordingInterfaceCanvas.hpp"
#include "RecordingNavigator.hpp"
#include "RecordingSettingsStorage.hpp"
#include "WidgetConstants.hpp"
#include "WidgetText.hpp"

using rtype::client::BoundAction;
using rtype::client::DIM_LAYER_COLOR;
using rtype::client::PRESS_DURATION;
using rtype::client::regularTextWidth;
using rtype::client::testing::ALONG_THE_TRACK;
using rtype::client::testing::CONTROLS_TAB;
using rtype::client::testing::DESCRIPTION_LEFT;
using rtype::client::testing::DESCRIPTION_RIGHT;
using rtype::client::testing::INDEX_OF_1280_BY_720;
using rtype::client::testing::LARGEST_FIT;
using rtype::client::testing::LONG_AFTER_A_FAILED_SAVE;
using rtype::client::testing::MUSIC_DEFAULT;
using rtype::client::testing::MUSIC_ONE_STEP_UP;
using rtype::client::testing::MUSIC_TRACK_START;
using rtype::client::testing::OPEN_LIST_LEFT;
using rtype::client::testing::QUIET_MUSIC;
using rtype::client::testing::ROWS_IN_THE_LONGEST_TAB;
using rtype::client::testing::SAVED_ANCHOR;
using rtype::client::testing::SAVED_STAMP_TIME;
using rtype::client::testing::TABS;
using rtype::client::testing::WINDOW_1280_BY_720;
using Key = sf::Keyboard::Key;
using Scan = sf::Keyboard::Scan;

/**
 * Given the options just opened over the title
 * When they are painted
 * Then the SOUND tab shows its four rows
 */
TEST_F(OptionsScreenFixture, TheOptionsOpenOnTheSoundTab) {
  EXPECT_NE(writingOf(canvas(), "MUSIC"), nullptr);
  EXPECT_NE(writingOf(canvas(), "EFFECTS"), nullptr);
  EXPECT_NE(writingOf(canvas(), "MUTE ALL"), nullptr);
  EXPECT_NE(writingOf(canvas(), "DEFAULT SOUND"), nullptr);
}

/**
 * Given the SOUND tab
 * When the key at the place of E is tapped
 * Then the CONTROLS tab shows its rows
 */
TEST_F(OptionsScreenFixture, TheNextTabKeyOpensControls) {
  openNextTab();
  EXPECT_NE(writingOf(canvas(), "MOVE UP"), nullptr);
}

/**
 * Given the SOUND tab
 * When the label of the CONTROLS tab is clicked
 * Then the CONTROLS tab shows its rows
 */
TEST_F(OptionsScreenFixture, AClickOnATabOpensIt) {
  screen().handleEvent(leftPress(CONTROLS_TAB * INTERFACE_PIXEL));
  screen().update(PRESS_DURATION);
  EXPECT_NE(writingOf(canvas(), "MOVE UP"), nullptr);
}

/**
 * Given MUSIC at 70, selected
 * When Right is tapped
 * Then the music is at 75 and those settings are saved once
 */
TEST_F(OptionsScreenFixture, RightOnMusicRaisesItAndSaves) {
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
TEST_F(OptionsScreenFixture, ASaveIsStampedSaved) {
  tap(Key::Right);
  const RecordedWriting* stamp = writingOf(canvas(), "SAVED");
  ASSERT_NE(stamp, nullptr);
  EXPECT_EQ(stamp->anchor, SAVED_ANCHOR);
}

/**
 * Given SAVED just stamped
 * When a second goes by
 * Then the stamp is gone
 */
TEST_F(OptionsScreenFixture, SavedFadesAfterASecond) {
  tap(Key::Right);
  screen().update(SAVED_STAMP_TIME);
  EXPECT_EQ(writingOf(canvas(), "SAVED"), nullptr);
}

/**
 * Given the options just opened, nothing changed
 * When frames go by
 * Then nothing is saved: the file is written only after a change
 */
TEST_F(OptionsScreenFixture, AFrameWithoutAChangeSavesNothing) {
  screen().update(PRESS_DURATION);
  screen().update(PRESS_DURATION);
  EXPECT_TRUE(storage().saves().empty());
}

/**
 * Given saves refused, as on a full disk
 * When the music is raised, then ten seconds go by
 * Then NOT SAVED stays on the screen
 */
TEST_F(OptionsScreenFixture, AFailedSaveStaysStampedNotSaved) {
  storage().refuseSaves(true);
  tap(Key::Right);
  screen().update(LONG_AFTER_A_FAILED_SAVE);
  EXPECT_NE(writingOf(canvas(), "NOT SAVED"), nullptr);
}

/**
 * Given the thumb of MUSIC pressed at the left of its track
 * When the pointer drags it right and a frame passes, the button still held
 * Then the volume dragged to is already saved, so closing the window then
 * loses nothing
 */
TEST_F(OptionsScreenFixture, ADraggedVolumeIsSavedBeforeTheRelease) {
  screen().handleEvent(leftPress(MUSIC_TRACK_START * INTERFACE_PIXEL));
  screen().handleEvent(
      pointerMove((MUSIC_TRACK_START + ALONG_THE_TRACK) * INTERFACE_PIXEL));
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
TEST_F(OptionsScreenFixture, AMutedVolumeSaysWhy) {
  settings().muted = true;
  screen().update(PRESS_DURATION);
  ASSERT_NE(descriptionOf(canvas()), nullptr);
  EXPECT_EQ(descriptionOf(canvas())->text, "> SOUND MUTED: SEE MUTE ALL");
}

/**
 * Given the music at 10
 * When DEFAULT SOUND is activated
 * Then a question is asked, and the volume is still 10
 */
TEST_F(OptionsScreenFixture, DefaultSoundAsksFirst) {
  settings().musicVolume = QUIET_MUSIC;
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Enter);
  EXPECT_NE(writingOf(canvas(), "PUT BACK THE DEFAULT SOUND?"), nullptr);
  EXPECT_EQ(settings().musicVolume, QUIET_MUSIC);
}

/**
 * Given the question of DEFAULT SOUND
 * When YES is chosen
 * Then the music is back at 70, saved, and the question is gone
 */
TEST_F(OptionsScreenFixture, YesPutsTheVolumesBack) {
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
  EXPECT_EQ(writingOf(canvas(), "PUT BACK THE DEFAULT SOUND?"), nullptr);
}

/**
 * Given the question of DEFAULT SOUND
 * When Escape is tapped
 * Then the volumes are kept, the question is gone and the options stay open
 */
TEST_F(OptionsScreenFixture, EscapeOnTheQuestionKeepsTheVolumes) {
  settings().musicVolume = QUIET_MUSIC;
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Down);
  tap(Key::Enter);
  tap(Key::Escape);
  EXPECT_EQ(settings().musicVolume, QUIET_MUSIC);
  EXPECT_EQ(writingOf(canvas(), "PUT BACK THE DEFAULT SOUND?"), nullptr);
  EXPECT_TRUE(navigator().calls().empty());
}

/**
 * Given MOVE UP selected in CONTROLS
 * When Enter is tapped, then W
 * Then W moves up, and the settings are saved
 */
TEST_F(OptionsScreenFixture, AKeyIsChangedByPressingTheNewOne) {
  openNextTab();
  tap(Key::Enter);
  ASSERT_NE(descriptionOf(canvas()), nullptr);
  EXPECT_EQ(descriptionOf(canvas())->text, "> PRESS THE NEW KEY. ESC CANCELS");
  tap(Key::W, Scan::W);
  EXPECT_EQ(settings().keys.keyOf(BoundAction::MoveUp), Scan::W);
  EXPECT_FALSE(storage().saves().empty());
}

/**
 * Given MOVE UP waiting for a key
 * When the Command key is pressed
 * Then it goes on waiting, and its description says the key cannot be bound
 */
TEST_F(OptionsScreenFixture, ARefusedKeySaysItCannotBeBound) {
  openNextTab();
  tap(Key::Enter);
  tap(Key::LSystem, Scan::LSystem);
  ASSERT_NE(descriptionOf(canvas()), nullptr);
  EXPECT_EQ(descriptionOf(canvas())->text, "> THIS KEY CANNOT BE BOUND");
  EXPECT_EQ(settings().keys.keyOf(BoundAction::MoveUp), Scan::Up);
}

/**
 * Given MOVE UP waiting for a key
 * When Escape is tapped
 * Then the key is kept and the options stay open
 */
TEST_F(OptionsScreenFixture, EscapeWhileWaitingKeepsTheKey) {
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
TEST_F(OptionsScreenFixture, AKeySharedByTwoActionsIsSignaled) {
  openNextTab();
  tap(Key::Enter);
  tap(Key::Space, Scan::Space);
  ASSERT_NE(descriptionOf(canvas()), nullptr);
  EXPECT_EQ(descriptionOf(canvas())->text, "> SAME KEY AS SHOOT: CHANGE ONE");
  EXPECT_NE(writingOf(canvas(), "!"), nullptr);
}

/**
 * Given the DISPLAY tab, the window at 1600 x 900
 * When Enter opens the list, Up then Enter take 1280 x 720
 * Then the window is 1280 x 720, and that size is saved
 */
TEST_F(OptionsScreenFixture, ASizeTakenResizesTheWindowAndIsSaved) {
  openNextTab();
  openNextTab();
  tap(Key::Enter);
  tap(Key::Up);
  tap(Key::Enter);
  EXPECT_EQ(window().windowSizeSelection().selectedIndex(),
            INDEX_OF_1280_BY_720);
  ASSERT_FALSE(storage().saves().empty());
  EXPECT_EQ(storage().saves().back().windowSize, WINDOW_1280_BY_720);
}

/**
 * Given the DISPLAY tab
 * When the list of sizes opens
 * Then everything it adds to the picture lies right of x 332, clear of the
 * chart and its legend on the left
 */
TEST_F(OptionsScreenFixture, TheOpenListLeavesThePreviewUncovered) {
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
TEST_F(OptionsScreenFixture, ASizeTooLargeCannotBeReached) {
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
TEST_F(OptionsScreenFixture, EveryDescriptionFitsBesideBack) {
  for (int tab = 0; tab < TABS; ++tab) {
    for (std::size_t row = 0; row < ROWS_IN_THE_LONGEST_TAB; ++row) {
      const RecordedWriting* line = descriptionOf(canvas());
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
TEST_F(OptionsScreenFixture, EscapeClosesTheOptions) {
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
TEST_F(OptionsScreenFixture, OverTheTitleTheOptionsHideIt) {
  EXPECT_TRUE(screen().hidesScreensBelow());
}

/**
 * Given the options over a game
 * When they are painted
 * Then the game shows, dimmed first, and THE GAME GOES ON is written
 */
TEST_F(GameOptionsScreenFixture, OverAGameTheOptionsDimItAndSaySo) {
  EXPECT_FALSE(screen().hidesScreensBelow());
  ASSERT_FALSE(canvas().fills.empty());
  EXPECT_EQ(canvas().fills.front().color, DIM_LAYER_COLOR);
  EXPECT_NE(writingOf(canvas(), "THE GAME GOES ON"), nullptr);
}
