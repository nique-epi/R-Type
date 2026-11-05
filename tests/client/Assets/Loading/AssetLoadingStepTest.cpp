#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <cstddef>
#include <filesystem>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "AssetLoadingStepTestConstants.hpp"
#include "AssetTestConstants.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadingException;
using rtype::client::AssetLoadingStep;
using rtype::client::AssetNotLoadedException;
using rtype::client::testing::BADLY_NAMED_ID;
using rtype::client::testing::BOSS_MUSIC_ID;
using rtype::client::testing::CORRUPT_FONT_ID;
using rtype::client::testing::NOT_AN_ASSET;
using rtype::client::testing::ONE_ASSET;
using rtype::client::testing::PLAYER_SHIP_ID;
using rtype::client::testing::STAGE_MUSIC_ID;
using rtype::client::testing::TWO_ASSETS;
using rtype::logging::genericText;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given an empty asset list
 * When its loading step starts
 * Then the step is already finished
 */
TEST(AssetLoadingStep, EmptyListIsFinishedAtOnce) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};

  const AssetLoadingStep step(library, AssetList{});

  EXPECT_TRUE(step.isFinished());
}

/**
 * Given a list of two music files
 * When the step loads once
 * Then exactly one of them is processed and the step goes on
 */
TEST(AssetLoadingStep, LoadsOneAssetPerCall) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(library,
                        AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}});

  step.loadNext();

  EXPECT_EQ(step.processedCount(), ONE_ASSET);
  EXPECT_EQ(step.assetCount(), TWO_ASSETS);
  EXPECT_FALSE(step.isFinished());
}

/**
 * Given a list of two music files
 * When the step loads twice
 * Then it is finished and both music files are located
 */
TEST(AssetLoadingStep, FinishesAfterTheLastAsset) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(library,
                        AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}});

  step.loadNext();
  step.loadNext();

  EXPECT_TRUE(step.isFinished());
  EXPECT_TRUE(library.isLoaded(AssetKind::Music, BOSS_MUSIC_ID));
}

/**
 * Given a finished step
 * When it is asked to load again
 * Then nothing happens and it stays finished
 */
TEST(AssetLoadingStep, LoadingAfterTheEndDoesNothing) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(library, AssetList{});

  step.loadNext();

  EXPECT_TRUE(step.isFinished());
  EXPECT_EQ(step.processedCount(), 0U);
}

/**
 * Given a music located for a previous game
 * When a step starts for a list that does not name it
 * Then the music is released
 */
TEST(AssetLoadingStep, ReleasesWhatTheListDoesNotName) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));

  const AssetLoadingStep step(library, AssetList{.music = {BOSS_MUSIC_ID}});

  EXPECT_THROW(static_cast<void>(library.musicFile(STAGE_MUSIC_ID)),
               AssetNotLoadedException);
}

/**
 * Given a music located for a previous game
 * When the next list names it too and its step finishes
 * Then the music is the same path, it was not located again
 */
TEST(AssetLoadingStep, KeepsWhatTheListNamesAgain) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  const std::filesystem::path* stageBeforeStep =
      &library.musicFile(STAGE_MUSIC_ID);
  AssetLoadingStep step(library, AssetList{.music = {STAGE_MUSIC_ID}});

  step.loadNext();

  EXPECT_EQ(&library.musicFile(STAGE_MUSIC_ID), stageBeforeStep);
}

/**
 * Given a list holding a badly named id, a missing texture, an unreadable
 * font and a music file
 * When the step loads every asset
 * Then the last load reports all three problems in one error
 */
TEST(AssetLoadingStep, ReportsEveryProblemAfterTheLastAsset) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(CORRUPT_FONT_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(library,
                        AssetList{.textures = {BADLY_NAMED_ID, PLAYER_SHIP_ID},
                                  .fonts = {CORRUPT_FONT_ID},
                                  .music = {STAGE_MUSIC_ID}});
  step.loadNext();
  step.loadNext();
  step.loadNext();

  EXPECT_THAT(
      [&step] { step.loadNext(); },
      ThrowsMessage<AssetLoadingException>(AllOf(
          HasSubstr(BADLY_NAMED_ID),
          HasSubstr(genericText(assets.path() / "sprites" / "player_ship.png")),
          HasSubstr(genericText(assets.path() / "fonts" / "corrupt.ttf")))));
}

/**
 * Given a music already located and a list naming it and another music
 * When the step starts
 * Then only the other music counts as an asset to load
 */
TEST(AssetLoadingStep, AssetsAlreadyLoadedAreNotCounted) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));

  const AssetLoadingStep step(
      library, AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}});

  EXPECT_EQ(step.assetCount(), ONE_ASSET);
}

/**
 * Given a located music made permanent
 * When a step starts for a list that does not name it
 * Then the music is still located
 */
TEST(AssetLoadingStep, KeepsPermanentAssetsItDoesNotName) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  library.keepPermanently(AssetList{.music = {STAGE_MUSIC_ID}});

  const AssetLoadingStep step(library, AssetList{.music = {BOSS_MUSIC_ID}});

  EXPECT_TRUE(library.isLoaded(AssetKind::Music, STAGE_MUSIC_ID));
}
