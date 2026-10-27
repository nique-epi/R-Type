#pragma once

#include <cstdint>

namespace rtype::client {

/**
 * @brief How the lights of the sky shine. Twinkling: each star twinkles one
 * step in STAR_TWINKLE_CYCLE_STEPS and the tail of a comet flickers. Steady,
 * for a player who turned REDUCE FLASHES on: no star twinkles, and the tail of
 * a comet keeps its gaps still and hides no pixel. The sky is given it at every
 * draw, so a change shows at the next frame. The background cannot read
 * FlashPreference, whose folder links it: this is its own word for it.
 */
enum class SkyLight : std::uint8_t {
  Twinkling,
  Steady,
};

}  // namespace rtype::client
