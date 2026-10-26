#pragma once

namespace rtype::client {

/** @brief A freeze of the intro: from @c at, in seconds of the music, the
 * scene stops for @c length seconds while the music, the shake and the texts
 * go on. */
struct IntroFreeze {
  float at{};
  float length{};
};

}  // namespace rtype::client
