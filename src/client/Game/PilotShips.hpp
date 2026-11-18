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

/** @brief Moves every ship of a pilot of @p components @p shift logical units
 * along x, as a transition of the level draws them flying out and back; their
 * places are written again from the server and the prediction each frame. */
void shiftPilotShips(engine::ComponentRegistry& components, float shift);

}  // namespace rtype::client
