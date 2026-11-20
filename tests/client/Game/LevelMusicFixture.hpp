#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <string_view>
#include "LevelMusic.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSky.hpp"
#include "RecordingSoundPlayer.hpp"

/**
 * @brief The music of a level over a sky whose biome WESTERN_BIOME names
 * WESTERN_MUSIC, playing on a recording audio service.
 */
class LevelMusicFixture : public ::testing::Test {
 protected:
  LevelMusicFixture();

  /** @brief Hands the music a LevelStatus of the biome @p biome. */
  void showBiome(std::string_view biome);

  /** @returns How many times the music of a game was asked for. */
  [[nodiscard]] std::size_t gameMusicCalls() const;

 private:
  RecordingSky sky_;
  RecordingSoundPlayer sounds_;
  RecordingAudioService audio_{sounds_};
  rtype::client::LevelMusic music_;
};
