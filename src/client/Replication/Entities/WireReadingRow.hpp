#pragma once

#include "WireReading.hpp"
#include "WorldState.hpp"

namespace rtype::client {

/** @brief One line of the table that says what a protocol entity type is. */
struct WireReadingRow {
  network::WireEntityType wireType{network::WireEntityType::PlayerShip};
  AnnouncedEntity entity;
};

}  // namespace rtype::client
