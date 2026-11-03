#include "NullSoundPlayer.hpp"
#include <string_view>

namespace rtype::client {

void NullSoundPlayer::play([[maybe_unused]] std::string_view soundId) {}

}  // namespace rtype::client
