#include <gtest/gtest.h>
#include <string_view>
#include "LevelMusicFixture.hpp"
#include "LevelMusicTestConstants.hpp"

/**
 * Given a biome whose background names a music
 * When the server repeats its status
 * Then the music is asked for once, so a repeated status restarts nothing
 */
TEST_F(LevelMusicFixture, ARepeatedStatusAsksTheMusicOnce) {
  showBiome(WESTERN_BIOME);
  showBiome(WESTERN_BIOME);

  EXPECT_EQ(gameMusicCalls(), ASKED_ONCE);
}

/**
 * Given a biome whose background names a music
 * When the level moves on to a biome that names none
 * Then the music is asked for again, the music of a game this time
 */
TEST_F(LevelMusicFixture, AnotherMusicIsAskedForWhenTheBiomeChanges) {
  showBiome(WESTERN_BIOME);
  showBiome(SILENT_BIOME);

  EXPECT_EQ(gameMusicCalls(), ASKED_TWICE);
}

/**
 * Given two biomes that name no music
 * When the level moves from one to the other
 * Then the music of a game is asked for once
 */
TEST_F(LevelMusicFixture, TwoBiomesWithoutMusicKeepTheMusicOfAGame) {
  showBiome(SILENT_BIOME);
  showBiome(std::string_view{});

  EXPECT_EQ(gameMusicCalls(), ASKED_ONCE);
}
