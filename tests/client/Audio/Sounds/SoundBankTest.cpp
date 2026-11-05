#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <filesystem>
#include "AssetLibrary.hpp"
#include "AssetTestConstants.hpp"
#include "ClientException.hpp"
#include "PathText.hpp"
#include "ScreenAssetsFixtures.hpp"
#include "SilentSound.hpp"
#include "SoundBank.hpp"
#include "SoundBankTestConstants.hpp"
#include "SoundConstants.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::ALL_SOUND_IDS;
using rtype::client::AssetLibrary;
using rtype::client::AssetLoadingException;
using rtype::client::AssetNotLoadedException;
using rtype::client::SoundBank;
using rtype::client::testing::BADLY_NAMED_SOUND_ID;
using rtype::client::testing::CORRUPT_SOUND_ID;
using rtype::client::testing::EXPLOSION_SOUND_ID;
using rtype::client::testing::LONG_SOUND_SAMPLES;
using rtype::client::testing::MISSING_SOUND_ID;
using rtype::client::testing::NOT_AN_ASSET;
using rtype::client::testing::ONLY_THE_SHOT;
using rtype::client::testing::SHORT_SOUND_SAMPLES;
using rtype::client::testing::SHOT_SOUND_ID;
using rtype::client::testing::THE_SHOT_AND_THE_EXPLOSION;
using rtype::client::testing::THREE_UNREADABLE_SOUNDS_AND_THE_SHOT;
using rtype::logging::genericText;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

/**
 * Given a sound file in a nested folder of the assets folder
 * When the bank loads it by its path in the folder
 * Then it finds it by that id, holding the samples of its file
 */
TEST(SoundBank, LoadsASoundAndFindsItById) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;

  bank.load(library, ONLY_THE_SHOT);

  EXPECT_EQ(bank.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a loaded sound
 * When its id is asked for twice
 * Then both answers are the same buffer
 */
TEST(SoundBank, SameIdGivesTheSameBuffer) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;
  bank.load(library, ONLY_THE_SHOT);

  const sf::SoundBuffer& first = bank.sound(SHOT_SOUND_ID);
  const sf::SoundBuffer& second = bank.sound(SHOT_SOUND_ID);

  EXPECT_EQ(&first, &second);
}

/**
 * Given a loaded sound whose file is then deleted
 * When its id is asked for
 * Then the loaded sound is returned, the disk is not read again
 */
TEST(SoundBank, LookUpsNeverReadTheDisk) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;
  bank.load(library, ONLY_THE_SHOT);

  std::filesystem::remove(assets.path() / SHOT_SOUND_ID);

  EXPECT_EQ(bank.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a loaded sound whose file is then replaced by a longer sound
 * When the bank loads it again
 * Then the first sound is kept, the file is read only once
 */
TEST(SoundBank, LoadingAgainKeepsTheFirstSound) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;
  bank.load(library, ONLY_THE_SHOT);
  writeSilentSound(assets.path() / SHOT_SOUND_ID, LONG_SOUND_SAMPLES);

  bank.load(library, ONLY_THE_SHOT);

  EXPECT_EQ(bank.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a file that holds no sound, a missing file, a badly named id and a
 * sound
 * When the bank loads the four of them
 * Then one error names the three it could not read
 */
TEST(SoundBank, ReportsEverySoundItCouldNotRead) {
  const TemporaryFolder assets;
  assets.write(CORRUPT_SOUND_ID, NOT_AN_ASSET);
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;

  EXPECT_THAT(([&bank, &library] {
                bank.load(library, THREE_UNREADABLE_SOUNDS_AND_THE_SHOT);
              }),
              ThrowsMessage<AssetLoadingException>(AllOf(
                  HasSubstr(genericText(library.fileOf(CORRUPT_SOUND_ID))),
                  HasSubstr(genericText(library.fileOf(MISSING_SOUND_ID))),
                  HasSubstr(BADLY_NAMED_SOUND_ID))));
}

/**
 * Given a load that met sounds it could not read before a sound it could
 * When the sound it could read is asked for
 * Then the bank holds it: every sound was tried
 */
TEST(SoundBank, AFailedLoadKeepsTheSoundsItRead) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;
  EXPECT_THROW(bank.load(library, THREE_UNREADABLE_SOUNDS_AND_THE_SHOT),
               AssetLoadingException);

  EXPECT_EQ(bank.sound(SHOT_SOUND_ID).getSampleCount(), SHORT_SOUND_SAMPLES);
}

/**
 * Given a bank that loaded the shot only
 * When the explosion is asked for
 * Then the error names the sound that is not loaded
 */
TEST(SoundBank, SoundThatIsNotLoadedIsNamed) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, LONG_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;
  bank.load(library, ONLY_THE_SHOT);

  EXPECT_THAT(
      [&bank] { static_cast<void>(bank.sound(EXPLOSION_SOUND_ID)); },
      ThrowsMessage<AssetNotLoadedException>(HasSubstr(EXPLOSION_SOUND_ID)));
}

/**
 * Given two sound files
 * When the bank loads both
 * Then each id gives its own file
 */
TEST(SoundBank, LoadsEverySoundOfTheList) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SHORT_SOUND_SAMPLES);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, LONG_SOUND_SAMPLES);
  const AssetLibrary library{assets.path()};
  SoundBank bank;

  bank.load(library, THE_SHOT_AND_THE_EXPLOSION);

  EXPECT_EQ(bank.sound(EXPLOSION_SOUND_ID).getSampleCount(),
            LONG_SOUND_SAMPLES);
}

/**
 * Given the assets folder of the repository
 * When the bank loads every sound of the client
 * Then every one of them can be looked up
 */
TEST(SoundBank, TheRepositoryHoldsEverySoundOfTheClient) {
  const AssetLibrary library{repositoryAssets()};
  SoundBank bank;

  bank.load(library, ALL_SOUND_IDS);

  for (const char* soundId : ALL_SOUND_IDS) {
    EXPECT_NO_THROW(static_cast<void>(bank.sound(soundId))) << soundId;
  }
}
