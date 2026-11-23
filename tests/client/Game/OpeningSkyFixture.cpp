#include "OpeningSkyFixture.hpp"
#include <string>
#include "GameStarted.hpp"
#include "OpeningSky.hpp"
#include "OpeningSkyTestConstants.hpp"
#include "RecordingAudioService.hpp"
#include "RecordingSky.hpp"

namespace rtype::client::testing {

OpeningSkyFixture::OpeningSkyFixture() {
  sky_.setBiomeMusic(std::string{CAVES_MUSIC});
}

void OpeningSkyFixture::announceTheCaves() {
  start_.hold(network::GameStarted{.biome = std::string{OPENING_BIOME},
                                   .scrollPosition = OPENING_SCROLL,
                                   .biomeSince = OPENING_BIOME_SINCE});
}

void OpeningSkyFixture::openTheLevel() {
  client::openTheLevel(start_, sky_, audio_);
}

const RecordingSky& OpeningSkyFixture::sky() const { return sky_; }

const RecordingAudioService& OpeningSkyFixture::audio() const { return audio_; }

}  // namespace rtype::client::testing
