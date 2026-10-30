#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief Whether the client makes sound: Enabled by default, Disabled when
 * the player launches it with NO_SOUND_OPTION. */
enum class AudioMode : std::uint8_t { Enabled, Disabled };

}  // namespace rtype::client
