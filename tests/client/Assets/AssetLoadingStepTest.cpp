#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <cstddef>
#include <cstdint>
#include "AssetIds.hpp"
#include "AssetKind.hpp"
#include "AssetLibrary.hpp"
#include "AssetList.hpp"
#include "AssetLoadingStep.hpp"
#include "ClientException.hpp"
#include "SilentSound.hpp"
#include "TemporaryFolder.hpp"

using rtype::client::AssetKind;
using rtype::client::AssetLibrary;
using rtype::client::AssetList;
using rtype::client::AssetLoadingException;
using rtype::client::AssetLoadingStep;
using rtype::client::AssetNotLoadedException;
using rtype::client::genericText;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::ThrowsMessage;

namespace {

constexpr const char* SHOT_SOUND_ID = "sounds/shot.wav";
constexpr const char* EXPLOSION_SOUND_ID = "sounds/explosion.wav";
constexpr const char* PLAYER_SHIP_ID = "sprites/player_ship.png";
constexpr const char* BADLY_NAMED_ID = "sprites/Bydo.png";
constexpr const char* CORRUPT_SOUND_ID = "sounds/corrupt.wav";
constexpr const char* CORRUPT_CONTENT = "not a sound";
constexpr std::uint64_t SOUND_SAMPLE_COUNT = 100;
constexpr std::size_t ONE_ASSET = 1;
constexpr std::size_t TWO_ASSETS = 2;

}  // namespace

/**
 * Given a next screen that lists no asset
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
 * Given a next screen that lists two sounds
 * When the step loads once
 * Then exactly one of them is processed and the step goes on
 */
TEST(AssetLoadingStep, LoadsOneAssetPerCall) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(
      library, AssetList{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}});

  step.loadNext();

  EXPECT_EQ(step.processedCount(), ONE_ASSET);
  EXPECT_EQ(step.assetCount(), TWO_ASSETS);
  EXPECT_FALSE(step.isFinished());
}

/**
 * Given a next screen that lists two sounds
 * When the step loads twice
 * Then it is finished and both sounds are loaded
 */
TEST(AssetLoadingStep, FinishesAfterTheLastAsset) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  writeSilentSound(assets.path() / EXPLOSION_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(
      library, AssetList{.sounds = {SHOT_SOUND_ID, EXPLOSION_SOUND_ID}});

  step.loadNext();
  step.loadNext();

  EXPECT_TRUE(step.isFinished());
  EXPECT_EQ(library.sound(EXPLOSION_SOUND_ID).getSampleCount(),
            SOUND_SAMPLE_COUNT);
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
 * Given a sound loaded for the previous screen
 * When a step starts for a next screen that does not list it
 * Then the sound is released
 */
TEST(AssetLoadingStep, ReleasesWhatTheNextScreenDoesNotList) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));

  const AssetLoadingStep step(library,
                              AssetList{.sounds = {EXPLOSION_SOUND_ID}});

  EXPECT_THROW(static_cast<void>(library.sound(SHOT_SOUND_ID)),
               AssetNotLoadedException);
}

/**
 * Given a sound loaded for the previous screen
 * When the next screen lists it too and its step finishes
 * Then the sound is the same buffer, it was not loaded again
 */
TEST(AssetLoadingStep, KeepsWhatBothScreensUse) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  AssetLibrary library{assets.path()};
  static_cast<void>(library.load(AssetKind::Sound, SHOT_SOUND_ID));
  const sf::SoundBuffer* shotBeforeStep = &library.sound(SHOT_SOUND_ID);
  AssetLoadingStep step(library, AssetList{.sounds = {SHOT_SOUND_ID}});

  step.loadNext();

  EXPECT_EQ(&library.sound(SHOT_SOUND_ID), shotBeforeStep);
}

/**
 * Given a next screen listing a badly named id, a missing texture, an
 * unreadable sound and a valid sound
 * When the step loads every asset
 * Then the last load reports all three problems in one error
 */
TEST(AssetLoadingStep, ReportsEveryProblemAfterTheLastAsset) {
  const TemporaryFolder assets;
  writeSilentSound(assets.path() / SHOT_SOUND_ID, SOUND_SAMPLE_COUNT);
  assets.write(CORRUPT_SOUND_ID, CORRUPT_CONTENT);
  AssetLibrary library{assets.path()};
  AssetLoadingStep step(library,
                        AssetList{.textures = {BADLY_NAMED_ID, PLAYER_SHIP_ID},
                                  .sounds = {CORRUPT_SOUND_ID, SHOT_SOUND_ID}});
  step.loadNext();
  step.loadNext();
  step.loadNext();

  EXPECT_THAT(
      [&step] { step.loadNext(); },
      ThrowsMessage<AssetLoadingException>(AllOf(
          HasSubstr(BADLY_NAMED_ID),
          HasSubstr(genericText(assets.path() / "sprites" / "player_ship.png")),
          HasSubstr(genericText(assets.path() / "sounds" / "corrupt.wav")))));
}
