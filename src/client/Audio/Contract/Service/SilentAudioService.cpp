#include "SilentAudioService.hpp"
#include <optional>
#include <string>
#include <string_view>
#include "Ambience.hpp"
#include "IMusicPlayer.hpp"
#include "ISoundPlayer.hpp"
#include "TimeConstants.hpp"

namespace rtype::client {

ISoundPlayer& SilentAudioService::sounds() { return sounds_; }

IMusicPlayer& SilentAudioService::theme() { return theme_; }

void SilentAudioService::playTheme() {}

void SilentAudioService::playGameMusic(
    [[maybe_unused]] const std::optional<std::string>& levelMusic) {}

void SilentAudioService::silenceMusic() {}

void SilentAudioService::playBossMusic(
    [[maybe_unused]] std::string_view music) {}

void SilentAudioService::setAmbience([[maybe_unused]] Ambience ambience) {}

Ambience SilentAudioService::ambience() const { return Ambience::None; }

void SilentAudioService::open() {}

void SilentAudioService::update([[maybe_unused]] engine::Duration elapsed) {}

}  // namespace rtype::client
