#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <vector>
#include "AssetIds.hpp"
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "ClientException.hpp"
#include "LoadingScreen.hpp"
#include "RecordingScreen.hpp"
#include "ScreenStack.hpp"
#include "SilentSound.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadingException;
using rtype::client::genericText;
using rtype::client::LoadingScreen;
using rtype::client::ScreenFactory;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::ThrowsMessage;

namespace {

constexpr const char* GAME = "game";
constexpr const char* SHOT_SOUND_ID = "sounds/shot.wav";
constexpr const char* EXPLOSION_SOUND_ID = "sounds/explosion.wav";
constexpr std::uint64_t SOUND_SAMPLE_COUNT = 100;

ScreenFactory loadingScreen(ScreenStack& screens, AssetLibrary& library,
                            const AssetList& assets,
                            std::vector<JournalEntry>& journal) {
  return [&screens, &library, &assets, &journal] {
    return std::make_unique<LoadingScreen>(screens, library, assets,
                                           recordingScreen(GAME, journal));
  };
}

}  // namespace

/**
 * Given a loading screen for two sounds
 * When one frame passes
 * Then exactly one of the two files has been read
 */
TEST(LoadingScreen, ReadsOneFilePerFrame) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  const AssetList twoSounds{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}};
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.replaceAll(loadingScreen(screens, library, twoSounds, journal));
  screens.applyPendingChanges();

  screens.update();

  EXPECT_NE(library.isLoaded(AssetKind::Sound, SHOT_SOUND_ID),
            library.isLoaded(AssetKind::Sound, EXPLOSION_SOUND_ID));
}

/**
 * Given a loading screen for two sounds
 * When one frame passes
 * Then the game is not shown yet
 */
TEST(LoadingScreen, WaitsForEveryFileBeforeShowingTheGame) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  const AssetList twoSounds{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}};
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.replaceAll(loadingScreen(screens, library, twoSounds, journal));
  screens.applyPendingChanges();

  screens.update();
  screens.applyPendingChanges();

  EXPECT_THAT(journal, IsEmpty());
}

/**
 * Given a loading screen for two sounds
 * When two frames pass
 * Then the game replaces the loading screen
 */
TEST(LoadingScreen, GivesWayToTheGameOnceEveryFileIsLoaded) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  const AssetList twoSounds{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}};
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.replaceAll(loadingScreen(screens, library, twoSounds, journal));
  screens.applyPendingChanges();

  screens.update();
  screens.applyPendingChanges();
  screens.update();
  screens.applyPendingChanges();

  EXPECT_THAT(journal, ElementsAre(JournalEntry{.screen = GAME,
                                                .call = ScreenCall::Built}));
}

/**
 * Given a loading screen with nothing to load
 * When the first frame passes
 * Then the game is shown at once
 */
TEST(LoadingScreen, NothingToLoadShowsTheGameAtTheFirstFrame) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};
  const AssetList nothing{};
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.replaceAll(loadingScreen(screens, library, nothing, journal));
  screens.applyPendingChanges();

  screens.update();
  screens.applyPendingChanges();

  EXPECT_THAT(journal, ElementsAre(JournalEntry{.screen = GAME,
                                                .call = ScreenCall::Built}));
}

/**
 * Given a loading screen for a sound whose file is missing
 * When the frame that loads it passes
 * Then an error naming the missing file stops the loading
 */
TEST(LoadingScreen, MissingFileStopsTheLoadingWithItsPath) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};
  const AssetList missingSound{.sounds = {SHOT_SOUND_ID}};
  std::vector<JournalEntry> journal;
  ScreenStack screens;
  screens.replaceAll(loadingScreen(screens, library, missingSound, journal));
  screens.applyPendingChanges();

  EXPECT_THAT([&screens] { screens.update(); },
              ThrowsMessage<AssetLoadingException>(HasSubstr(
                  genericText(assets.path() / "sounds" / "shot.wav"))));
}
