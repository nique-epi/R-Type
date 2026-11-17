#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief The sound that colors a screen: nothing, deep space on the server
 * map and a garage in the hangar, which play under the theme; the music of
 * the shop and the two musics of its radio, the theme of Papers, Please and
 * Kavorka, which play instead of the theme, paused for as long as one of
 * them plays. */
enum class Ambience : std::uint8_t {
  None,
  Space,
  Hangar,
  Shop,
  ShopRadio,
  ShopDance,
};

}  // namespace rtype::client
