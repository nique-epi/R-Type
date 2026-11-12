#pragma once

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Moves every body with a ScrollMotion and a Position @p travel logical
 * units to the left: as far as the level scrolled since the last call, 0 while
 * it is paused.
 */
void carryAlongTheScroll(engine::ComponentRegistry& components, float travel);

}  // namespace rtype::game
