#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <filesystem>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetLibraryTestConstants.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "AssetTestConstants.hpp"
#include "ClientException.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadResult;
using rtype::client::AssetNotLoadedException;
using rtype::client::testing::ASSETS_FOLDER;
using rtype::client::testing::BOSS_MUSIC_ID;
using rtype::client::testing::CORRUPT_ID;
using rtype::client::testing::ESCAPING_MUSIC_ID;
using rtype::client::testing::HUD_FONT_ID;
using rtype::client::testing::NOT_AN_ASSET;
using rtype::client::testing::OUTSIDE_FILE_NAME;
using rtype::client::testing::PLAYER_SHIP_ID;
using rtype::client::testing::STAGE_MUSIC_ID;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given an empty assets folder
 * When a texture is loaded
 * Then its file is reported missing
 */
TEST(AssetLibrary, ReportsAMissingFile) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};

  EXPECT_EQ(library.load(AssetKind::Texture, PLAYER_SHIP_ID),
            AssetLoadResult::MissingFile);
}

/**
 * Given a music file outside the assets folder
 * When it is loaded through an id that climbs out of the folder
 * Then the id is rejected instead of the file being located
 */
TEST(AssetLibrary, RejectsAnIdThatLeavesTheFolder) {
  const TemporaryFolder repository;
  repository.write(OUTSIDE_FILE_NAME, NOT_AN_ASSET);
  AssetLibrary library{repository.path() / ASSETS_FOLDER};

  EXPECT_EQ(library.load(AssetKind::Music, ESCAPING_MUSIC_ID),
            AssetLoadResult::InvalidId);
}

/**
 * Given a file that holds no font
 * When it is loaded as a font
 * Then it is reported unreadable
 */
TEST(AssetLibrary, ReportsAnUnreadableFont) {
  const TemporaryFolder assets;
  assets.write(CORRUPT_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};

  EXPECT_EQ(library.load(AssetKind::Font, CORRUPT_ID),
            AssetLoadResult::UnreadableFile);
}

/**
 * Given a music file that holds no music
 * When it is loaded
 * Then it is not read, and its id gives its path
 */
TEST(AssetLibrary, LocatesMusicWithoutReadingIt) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};

  const AssetLoadResult result = library.load(AssetKind::Music, STAGE_MUSIC_ID);

  EXPECT_EQ(result, AssetLoadResult::Loaded);
  EXPECT_EQ(library.musicFile(STAGE_MUSIC_ID),
            assets.path() / "music" / "stage.ogg");
}

/**
 * Given a library where nothing is loaded
 * When a texture is asked for
 * Then the error names the asset that is not loaded
 */
TEST(AssetLibrary, TextureThatIsNotLoadedIsNamed) {
  const TemporaryFolder assets;
  const AssetLibrary library{assets.path()};

  EXPECT_THAT(
      [&library] { static_cast<void>(library.texture(PLAYER_SHIP_ID)); },
      ThrowsMessage<AssetNotLoadedException>(HasSubstr(PLAYER_SHIP_ID)));
}

/**
 * Given a library where nothing is loaded
 * When a texture is looked for without throwing
 * Then none is found
 */
TEST(AssetLibrary, FindsNoTextureThatIsNotLoaded) {
  const TemporaryFolder assets;
  const AssetLibrary library{assets.path()};

  EXPECT_EQ(library.findTexture(PLAYER_SHIP_ID), nullptr);
}

/**
 * Given a texture whose file is missing
 * When it is loaded, then looked for without throwing
 * Then none is found: a failed load leaves nothing behind
 */
TEST(AssetLibrary, FindsNoTextureWhoseLoadFailed) {
  const TemporaryFolder assets;
  AssetLibrary library{assets.path()};
  ASSERT_EQ(library.load(AssetKind::Texture, PLAYER_SHIP_ID),
            AssetLoadResult::MissingFile);

  EXPECT_EQ(library.findTexture(PLAYER_SHIP_ID), nullptr);
}

/**
 * Given a library where nothing is loaded
 * When a font is asked for
 * Then the error names the asset that is not loaded
 */
TEST(AssetLibrary, FontThatIsNotLoadedIsNamed) {
  const TemporaryFolder assets;
  const AssetLibrary library{assets.path()};

  EXPECT_THAT([&library] { static_cast<void>(library.font(HUD_FONT_ID)); },
              ThrowsMessage<AssetNotLoadedException>(HasSubstr(HUD_FONT_ID)));
}

/**
 * Given two located music files
 * When every asset but the first is released
 * Then the first is still the same path
 */
TEST(AssetLibrary, ReleaseKeepsTheListedAssets) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  static_cast<void>(library.load(AssetKind::Music, BOSS_MUSIC_ID));
  const std::filesystem::path* stageBeforeRelease =
      &library.musicFile(STAGE_MUSIC_ID);

  library.releaseAllExcept(AssetList{.music = {STAGE_MUSIC_ID}});

  EXPECT_EQ(&library.musicFile(STAGE_MUSIC_ID), stageBeforeRelease);
}

/**
 * Given two located music files
 * When every asset but the first is released
 * Then the second is no longer loaded
 */
TEST(AssetLibrary, ReleaseDropsTheOtherAssets) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  static_cast<void>(library.load(AssetKind::Music, BOSS_MUSIC_ID));

  library.releaseAllExcept(AssetList{.music = {STAGE_MUSIC_ID}});

  EXPECT_THROW(static_cast<void>(library.musicFile(BOSS_MUSIC_ID)),
               AssetNotLoadedException);
}

/**
 * Given a music file that was never loaded
 * When the library is asked whether that music is loaded
 * Then it answers no
 */
TEST(AssetLibrary, AssetNeverLoadedIsNotLoaded) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  const AssetLibrary library{assets.path()};

  EXPECT_FALSE(library.isLoaded(AssetKind::Music, STAGE_MUSIC_ID));
}

/**
 * Given a located music file
 * When the library is asked whether that music is loaded
 * Then it answers yes
 */
TEST(AssetLibrary, LoadedAssetIsLoaded) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));

  EXPECT_TRUE(library.isLoaded(AssetKind::Music, STAGE_MUSIC_ID));
}

/**
 * Given a located music file made permanent
 * When every asset is released
 * Then the music is still the same path
 */
TEST(AssetLibrary, ReleaseKeepsPermanentAssets) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  const std::filesystem::path* stageBeforeRelease =
      &library.musicFile(STAGE_MUSIC_ID);
  library.keepPermanently(AssetList{.music = {STAGE_MUSIC_ID}});

  library.releaseAllExcept(AssetList{});

  EXPECT_EQ(&library.musicFile(STAGE_MUSIC_ID), stageBeforeRelease);
}

/**
 * Given a music made permanent before it was located, then located
 * When every asset is released
 * Then the music is still located
 */
TEST(AssetLibrary, PermanenceAppliesToAssetsLoadedLater) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  library.keepPermanently(AssetList{.music = {STAGE_MUSIC_ID}});
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));

  library.releaseAllExcept(AssetList{});

  EXPECT_TRUE(library.isLoaded(AssetKind::Music, STAGE_MUSIC_ID));
}

/**
 * Given a library with nothing loaded
 * When it is asked whether an empty list is loaded
 * Then it answers yes, since there is nothing to load
 */
TEST(AssetLibrary, EmptyListIsLoaded) {
  const TemporaryFolder assets;
  const AssetLibrary library{assets.path()};

  EXPECT_TRUE(library.isLoaded(AssetList{}));
}

/**
 * Given two music files of which only one is located
 * When the library is asked whether the list of both is loaded
 * Then it answers no
 */
TEST(AssetLibrary, ListIsNotLoadedWhileOneAssetIsMissing) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));

  EXPECT_FALSE(
      library.isLoaded(AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}}));
}

/**
 * Given two located music files
 * When the library is asked whether the list of both is loaded
 * Then it answers yes
 */
TEST(AssetLibrary, ListIsLoadedOnceEveryAssetIs) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, NOT_AN_ASSET);
  assets.write(BOSS_MUSIC_ID, NOT_AN_ASSET);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Music, STAGE_MUSIC_ID));
  static_cast<void>(library.load(AssetKind::Music, BOSS_MUSIC_ID));

  EXPECT_TRUE(
      library.isLoaded(AssetList{.music = {STAGE_MUSIC_ID, BOSS_MUSIC_ID}}));
}
