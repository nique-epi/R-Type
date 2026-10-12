#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>
#include "AssetIds.hpp"
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "ClientException.hpp"
#include "IScreen.hpp"
#include "LoadingScreen.hpp"
#include "RecordingScreen.hpp"
#include "ScreenId.hpp"
#include "ScreenStack.hpp"
#include "SilentSound.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadingException;
using rtype::client::genericText;
using rtype::client::IScreen;
using rtype::client::LoadingScreen;
using rtype::client::ScreenId;
using rtype::client::ScreenStack;
using ::testing::ElementsAre;
using ::testing::HasSubstr;
using ::testing::IsEmpty;
using ::testing::ThrowsMessage;

namespace {

constexpr const char* SHOT_SOUND_ID = "sounds/shot.wav";
constexpr const char* EXPLOSION_SOUND_ID = "sounds/explosion.wav";
constexpr std::uint64_t SOUND_SAMPLE_COUNT = 100;

/** @brief A stack whose first screen is a loading screen for toLoad_, and
 * whose next screens are recording doubles. */
class LoadingScreenTest : public ::testing::Test {
 protected:
  void loadTwoSounds() {
    writeSilentSound(folder_.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
    writeSilentSound(folder_.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
    toLoad_ = AssetList{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}};
  }

  void showLoadingScreenOf(ScreenId screen) {
    screens_.replaceAll(screen);
    screens_.applyPendingChanges();
  }

  void runFrame() {
    screens_.update();
    screens_.applyPendingChanges();
  }

  void loadOnly(const AssetList& assets) { toLoad_ = assets; }

  ScreenStack& screens() { return screens_; }

  [[nodiscard]] const AssetLibrary& library() const { return library_; }

  [[nodiscard]] const std::filesystem::path& folder() const {
    return folder_.path();
  }

  [[nodiscard]] const std::vector<JournalEntry>& journal() const {
    return journal_;
  }

 private:
  TemporaryFolder folder_;
  AssetLibrary library_{folder_.path()};
  AssetList toLoad_;
  bool loadingScreenBuilt_ = false;
  std::vector<JournalEntry> journal_;
  ScreenStack screens_{[this](ScreenId screen) -> std::unique_ptr<IScreen> {
    if (!loadingScreenBuilt_) {
      loadingScreenBuilt_ = true;
      return std::make_unique<LoadingScreen>(screens_, library_, toLoad_,
                                             screen);
    }
    return std::make_unique<RecordingScreen>(screen, journal_);
  }};
};

}  // namespace

/**
 * Given a loading screen for two sounds
 * When one frame passes
 * Then exactly one of the two files has been read
 */
TEST_F(LoadingScreenTest, ReadsOneFilePerFrame) {
  loadTwoSounds();
  showLoadingScreenOf(ScreenId::Game);

  screens().update();

  EXPECT_NE(library().isLoaded(AssetKind::Sound, SHOT_SOUND_ID),
            library().isLoaded(AssetKind::Sound, EXPLOSION_SOUND_ID));
}

/**
 * Given a loading screen for two sounds
 * When one frame passes
 * Then its screen is not shown yet
 */
TEST_F(LoadingScreenTest, WaitsForEveryFileBeforeShowingItsScreen) {
  loadTwoSounds();
  showLoadingScreenOf(ScreenId::Game);

  runFrame();

  EXPECT_THAT(journal(), IsEmpty());
}

/**
 * Given a loading screen for two sounds
 * When two frames pass
 * Then its screen replaces it
 */
TEST_F(LoadingScreenTest, GivesWayToItsScreenOnceEveryFileIsLoaded) {
  loadTwoSounds();
  showLoadingScreenOf(ScreenId::Game);

  runFrame();
  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen for the options, with nothing to load
 * When the first frame passes
 * Then the options are shown, by their identifier
 */
TEST_F(LoadingScreenTest, AsksForTheScreenItLoadsFor) {
  showLoadingScreenOf(ScreenId::Options);

  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = ScreenId::Options,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen with nothing to load
 * When the first frame passes
 * Then its screen is shown at once
 */
TEST_F(LoadingScreenTest, NothingToLoadShowsItsScreenAtTheFirstFrame) {
  showLoadingScreenOf(ScreenId::Game);

  runFrame();

  EXPECT_THAT(journal(), ElementsAre(JournalEntry{.screen = ScreenId::Game,
                                                  .call = ScreenCall::Built}));
}

/**
 * Given a loading screen that gave way to its screen
 * When that screen leaves the stack
 * Then the stack is empty, the loading screen being gone
 */
TEST_F(LoadingScreenTest, LeavesTheStackOnceItsScreenIsShown) {
  showLoadingScreenOf(ScreenId::Game);
  runFrame();

  screens().pop();
  screens().applyPendingChanges();

  EXPECT_TRUE(screens().isEmpty());
}

/**
 * Given a loading screen for a sound whose file is missing
 * When the frame that loads it passes
 * Then an error naming the missing file stops the loading
 */
TEST_F(LoadingScreenTest, MissingFileStopsTheLoadingWithItsPath) {
  loadOnly(AssetList{.sounds = {SHOT_SOUND_ID}});
  showLoadingScreenOf(ScreenId::Game);

  EXPECT_THAT([this] { screens().update(); },
              ThrowsMessage<AssetLoadingException>(
                  HasSubstr(genericText(folder() / "sounds" / "shot.wav"))));
}
