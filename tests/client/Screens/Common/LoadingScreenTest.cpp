#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "AssetKind.hpp"
#include "AssetList.hpp"
#include "AssetTestConstants.hpp"
#include "ClientException.hpp"
#include "GameScreenIdConstants.hpp"
#include "LoadingScreenFixture.hpp"
#include "MenuScreenIdConstants.hpp"
#include "PathText.hpp"
#include "RecordingScreen.hpp"
#include "ScreenTestConstants.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetList;
using rtype::client::AssetLoadingException;
using rtype::client::GAME_SCREEN;
using rtype::client::OPTIONS_SCREEN;
using rtype::client::testing::BOSS_MUSIC_ID;
using rtype::client::testing::STAGE_MUSIC_ID;
using rtype::engine::genericText;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::ThrowsMessage;

/**
 * Given a loading screen for two music files
 * When one frame passes
 * Then exactly one of the two files has been read
 */
TEST_F(LoadingScreenFixture, ReadsOneFilePerFrame) {
  loadTwoMusicFiles();
  showLoadingScreenOf(GAME_SCREEN);

  screens().update(ONE_FRAME);

  EXPECT_NE(library().isLoaded(AssetKind::Music, STAGE_MUSIC_ID),
            library().isLoaded(AssetKind::Music, BOSS_MUSIC_ID));
}

/**
 * Given a loading screen for two music files
 * When one frame passes
 * Then its screen is not shown yet
 */
TEST_F(LoadingScreenFixture, WaitsForEveryFileBeforeShowingItsScreen) {
  loadTwoMusicFiles();
  showLoadingScreenOf(GAME_SCREEN);

  runFrame();

  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given a loading screen for two music files
 * When two frames pass
 * Then its screen replaces it
 */
TEST_F(LoadingScreenFixture, GivesWayToItsScreenOnceEveryFileIsLoaded) {
  loadTwoMusicFiles();
  showLoadingScreenOf(GAME_SCREEN);

  runFrame();
  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = GAME_SCREEN,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen for the options, with nothing to load
 * When the first frame passes
 * Then the options are shown, by their identifier
 */
TEST_F(LoadingScreenFixture, AsksForTheScreenItLoadsFor) {
  showLoadingScreenOf(OPTIONS_SCREEN);

  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = OPTIONS_SCREEN,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen with nothing to load
 * When the first frame passes
 * Then its screen is shown at once
 */
TEST_F(LoadingScreenFixture, NothingToLoadShowsItsScreenAtTheFirstFrame) {
  showLoadingScreenOf(GAME_SCREEN);

  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = GAME_SCREEN,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen that gave way to its screen
 * When that screen leaves the stack
 * Then the stack is empty, the loading screen being gone
 */
TEST_F(LoadingScreenFixture, LeavesTheStackOnceItsScreenIsShown) {
  showLoadingScreenOf(GAME_SCREEN);
  runFrame();

  screens().pop();
  screens().applyPendingChanges();

  EXPECT_TRUE(screens().isEmpty());
}

/**
 * Given a loading screen for a music whose file is missing
 * When the frame that loads it passes
 * Then an error naming the missing file stops the loading
 */
TEST_F(LoadingScreenFixture, MissingFileStopsTheLoadingWithItsPath) {
  loadOnly(AssetList{.music = {STAGE_MUSIC_ID}});
  showLoadingScreenOf(GAME_SCREEN);

  EXPECT_THAT([this] { screens().update(ONE_FRAME); },
              ThrowsMessage<AssetLoadingException>(
                  HasSubstr(genericText(folder() / "music" / "stage.ogg"))));
}
