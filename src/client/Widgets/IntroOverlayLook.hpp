#pragma once

#include <functional>
#include "PlayerPalette.hpp"
#include "TextureArea.hpp"

namespace rtype::client {

/**
 * @brief What the overlay of the intro paints with: the whole texture of the
 * logo of the title, and the palette the nicknames of the pilots are written
 * in.
 *
 * A std::reference_wrapper has no default constructor, so this struct is
 * only ever built with designated initializers naming every member: the
 * member-init checks, which report its implicit default constructor, see a
 * constructor that cannot be called.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct IntroOverlayLook {
  TextureArea logo;
  std::reference_wrapper<const PlayerPalette> palette;
};

}  // namespace rtype::client
