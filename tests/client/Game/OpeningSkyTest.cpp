#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include "MusicCall.hpp"
#include "OpeningSkyFixture.hpp"
#include "OpeningSkyTestConstants.hpp"
#include "SkyLevel.hpp"

using rtype::client::SkyLevel;
using rtype::client::testing::OPENING_BIOME;
using rtype::client::testing::OPENING_BIOME_SINCE;
using rtype::client::testing::OPENING_SCROLL;
using rtype::client::testing::OpeningSkyFixture;
using ::testing::ElementsAre;

/**
 * Given a game announced in the caves, its scroll at 240 units, the caves
 * begun at -4096
 * When the level is opened before the game loads
 * Then the sky shows the caves there, its scroll still
 */
TEST_F(OpeningSkyFixture, ShowsTheSkyTheLevelOpensIn) {
  announceTheCaves();

  openTheLevel();

  EXPECT_EQ(sky().lastLevel(), (SkyLevel{.biome = std::string{OPENING_BIOME},
                                         .scrollPosition = OPENING_SCROLL,
                                         .scrollSpeed = 0.0F,
                                         .biomeSince = OPENING_BIOME_SINCE}));
}

/**
 * Given a game announced in the caves, whose background names a music
 * When the level is opened before the game loads
 * Then the music the level names plays, never the music of a game
 */
TEST_F(OpeningSkyFixture, PlaysTheMusicOfTheBiomeItOpensIn) {
  announceTheCaves();

  openTheLevel();

  EXPECT_THAT(audio().musicCalls(), ElementsAre(MusicCall::LevelMusic));
}

/**
 * Given a game started without a GameStarted, its copies lost
 * When the level is opened before the game loads
 * Then the sky is left without a biome and the music of a game plays
 */
TEST_F(OpeningSkyFixture, WithoutAStartLeavesTheSkyAndPlaysTheGameMusic) {
  openTheLevel();

  EXPECT_EQ(sky().lastLevel(), SkyLevel{});
  EXPECT_THAT(audio().musicCalls(), ElementsAre(MusicCall::GameMusic));
}
