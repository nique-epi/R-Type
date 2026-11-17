#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief What a layer of an LDtk project holds: tiles on its grid, or props,
 * entities placed one by one. */
enum class MapLayerKind : std::uint8_t { Tiles, Props };

}  // namespace rtype::client
