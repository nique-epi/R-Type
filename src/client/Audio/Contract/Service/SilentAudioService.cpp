#include "SilentAudioService.hpp"
#include "Ambience.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ISoundPlayer& SilentAudioService::sounds() { return sounds_; }

IMusicPlayer& SilentAudioService::theme() { return theme_; }

void SilentAudioService::playTheme() {}

void SilentAudioService::playGameMusic() {}

void SilentAudioService::silenceMusic() {}

void SilentAudioService::playBossMusic() {}

void SilentAudioService::setAmbience([[maybe_unused]] Ambience ambience) {}

Ambience SilentAudioService::ambience() const { return Ambience::None; }

void SilentAudioService::open() {}

void SilentAudioService::update([[maybe_unused]] engine::Duration elapsed) {}

}  // namespace rtype::client
