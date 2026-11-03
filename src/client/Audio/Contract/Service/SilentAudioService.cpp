#include "SilentAudioService.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ISoundPlayer& SilentAudioService::sounds() { return sounds_; }

IMusicPlayer& SilentAudioService::theme() { return theme_; }

void SilentAudioService::playTheme() {}

void SilentAudioService::playGameMusic() {}

void SilentAudioService::open() {}

void SilentAudioService::update([[maybe_unused]] engine::Duration elapsed) {}

}  // namespace rtype::client
