#pragma once

#include "ShipLook.hpp"

namespace rtype::client {

/** @brief Component of the ship of a pilot: the items it wears, which
 * RenderSystem draws over the ship. */
struct WornLook {
  identifiers::ShipLook look;
};

}  // namespace rtype::client
