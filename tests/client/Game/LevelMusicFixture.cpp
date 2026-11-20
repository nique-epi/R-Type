#include "LevelMusicFixture.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include "LevelMusicTestConstants.hpp"
#include "LevelStatus.hpp"
#include "MusicCall.hpp"

LevelMusicFixture::LevelMusicFixture() : music_(sky_, audio_) {
  sky_.giveMusic(std::string{WESTERN_BIOME}, std::string{WESTERN_MUSIC});
}

void LevelMusicFixture::showBiome(std::string_view biome) {
  music_.follow(rtype::network::LevelStatus{.biome = std::string{biome}});
}

std::size_t LevelMusicFixture::gameMusicCalls() const {
  return static_cast<std::size_t>(
      std::ranges::count(audio_.musicCalls(), MusicCall::GameMusic));
}
