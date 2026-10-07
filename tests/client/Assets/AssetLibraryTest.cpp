#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <cstdint>
#include <filesystem>
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadResult.hpp"
#include "ClientException.hpp"
#include "SilentSound.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadResult;
using rtype::client::AssetNotLoadedException;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

namespace {

constexpr const char* ASSETS_FOLDER = "assets";
constexpr const char* SHOT_SOUND_ID = "sounds/weapons/shot.wav";
constexpr const char* EXPLOSION_SOUND_ID = "sounds/explosion.wav";
constexpr const char* PLAYER_SHIP_ID = "sprites/player_ship.png";
constexpr const char* HUD_FONT_ID = "fonts/hud.ttf";
constexpr const char* STAGE_MUSIC_ID = "music/stage.ogg";
constexpr const char* CORRUPT_ID = "corrupt/asset.bin";
constexpr const char* OUTSIDE_FILE_NAME = "outside.wav";
constexpr const char* ESCAPING_SOUND_ID = "../outside.wav";
constexpr const char* CORRUPT_CONTENT = "neither an image, a font nor a sound";
constexpr std::uint64_t SHORT_SOUND_SAMPLES = 100;
constexpr std::uint64_t LONG_SOUND_SAMPLES = 200;

class UnreadableSoundOrFont : public ::testing::TestWithParam<AssetKind> {};

}  // namespace

/**
 * Given a sound file in a nested folder of sounds
 * When it is loaded by its path in the folder
 * Then it is found by that id and holds the samples of its file
 */
TEST(AssetLibrary, LoadsASoundAndFindsItById) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};

  const AssetLoadResult result = library.load(AssetKind::Sound, SHOT_SOUND_ID);

  EXPECT_EQ(result, AssetLoadResult::Loaded);
  EXPECT_EQ(library.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a loaded sound
 * When its id is asked for twice
 * Then both answers are the same buffer
 */
TEST(AssetLibrary, SameIdGivesTheSameResource) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));

  const sf::SoundBuffer& first = library.sound(SHOT_SOUND_ID);
  const sf::SoundBuffer& second = library.sound(SHOT_SOUND_ID);

  EXPECT_EQ(&first, &second);
}

/**
 * Given a loaded sound
 * When its file is deleted and its id asked for
 * Then the loaded sound is returned, the disk is not read again
 */
TEST(AssetLibrary, LookUpsNeverReadTheDisk) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));

  std::filesystem::remove(assets.path() / SHOT_SOUND_ID);

  EXPECT_EQ(library.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a loaded sound whose file is then replaced by a longer sound
 * When it is loaded again
 * Then the first sound is kept, the file is read only once
 */
TEST(AssetLibrary, LoadingAgainKeepsTheFirstResource) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));
  writeSilentSound(assets.path() / SHOT_SOUND_ID, LONG_SOUND_SAMPLES);

  const AssetLoadResult result = library.load(AssetKind::Sound, SHOT_SOUND_ID);

  EXPECT_EQ(result, AssetLoadResult::Loaded);
  EXPECT_EQ(library.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

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
 * Given a sound file outside the assets folder
 * When it is loaded through an id that climbs out of the folder
 * Then the id is rejected instead of the file being read
 */
TEST(AssetLibrary, RejectsAnIdThatLeavesTheFolder) {
  const TemporaryFolder repository;
  writeSilentSound(repository.path() / OUTSIDE_FILE_NAME, SHORT_SOUND_SAMPLES);
  AssetLibrary library{repository.path() / ASSETS_FOLDER};

  EXPECT_EQ(library.load(AssetKind::Sound, ESCAPING_SOUND_ID),
            AssetLoadResult::InvalidId);
}

/**
 * Given a file that holds no sound or font
 * When it is loaded as one of them
 * Then it is reported unreadable
 */
TEST_P(UnreadableSoundOrFont, IsReportedUnreadable) {
  const TemporaryFolder assets;
  assets.write(CORRUPT_ID, CORRUPT_CONTENT);
  AssetLibrary library{assets.path()};

  EXPECT_EQ(library.load(GetParam(), CORRUPT_ID),
            AssetLoadResult::UnreadableFile);
}

INSTANTIATE_TEST_SUITE_P(KindsLoadedWithoutADisplay, UnreadableSoundOrFont,
                         ::testing::Values(AssetKind::Sound, AssetKind::Font));

/**
 * Given a music file that holds no music
 * When it is loaded
 * Then it is not read, and its id gives its path
 */
TEST(AssetLibrary, LocatesMusicWithoutReadingIt) {
  const TemporaryFolder assets;
  assets.write(STAGE_MUSIC_ID, CORRUPT_CONTENT);
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
 * Given two loaded sounds
 * When every asset but the first is released
 * Then the first is still the same buffer
 */
TEST(AssetLibrary, ReleaseKeepsTheListedAssets) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, LONG_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));
  static_cast<void>(library.load(AssetKind::Sound, EXPLOSION_SOUND_ID));
  const sf::SoundBuffer* shotBeforeRelease = &library.sound(SHOT_SOUND_ID);

  library.releaseAllExcept(AssetList{.sounds = {SHOT_SOUND_ID}});

  EXPECT_EQ(&library.sound(SHOT_SOUND_ID), shotBeforeRelease);
}

/**
 * Given two loaded sounds
 * When every asset but the first is released
 * Then the second is no longer loaded
 */
TEST(AssetLibrary, ReleaseDropsTheOtherAssets) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, LONG_SOUND_SAMPLES);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));
  static_cast<void>(library.load(AssetKind::Sound, EXPLOSION_SOUND_ID));

  library.releaseAllExcept(AssetList{.sounds = {SHOT_SOUND_ID}});

  EXPECT_THROW(static_cast<void>(library.sound(EXPLOSION_SOUND_ID)),
               AssetNotLoadedException);
}
