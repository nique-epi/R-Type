#pragma once

#include <array>
#include <cstddef>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntitySpriteConstants.hpp"

namespace rtype::client {

/** @brief The ships of the pilots, at most PLAYER_COLOR_COUNT: gathered first,
 * since forEach forbids adding a component while it runs. */
struct PilotShips {
  std::array<engine::Entity, PLAYER_COLOR_COUNT> ships{};
  std::size_t count{};
};

/** @returns Every alive entity with a PilotShip and a Sprite, up to
 * PLAYER_COLOR_COUNT; a ship past that count waits for the next frame. */
[[nodiscard]] PilotShips pilotShipsOf(engine::ComponentRegistry& components);

}  // namespace rtype::client
