#include "AmbienceScope.hpp"
#include "Ambience.hpp"
#include "IAudioService.hpp"

namespace rtype::client {

AmbienceScope::AmbienceScope(IAudioService& audio, Ambience ambience)
    : audio_(&audio), before_(audio.ambience()) {
  audio_->setAmbience(ambience);
}

AmbienceScope::~AmbienceScope() { audio_->setAmbience(before_); }

}  // namespace rtype::client
