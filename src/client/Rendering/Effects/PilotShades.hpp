#pragma once

#include <functional>
#include "PilotDebrisRamps.hpp"
#include "PlayerPalette.hpp"

namespace rtype::client {

/**
 * @brief The shades the effects paint pilots in: those of the palette the
 * player chose, and the debris ramps made of them. Both are constants, so the
 * spans particles keep into a ramp never dangle.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct PilotShades {
  std::reference_wrapper<const PlayerPalette> palette;
  std::reference_wrapper<const PilotDebrisRamps> debris;
};

}  // namespace rtype::client
