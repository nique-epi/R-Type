#pragma once

#include <span>
#include "ComponentRegistry.hpp"
#include "PlayerLooks.hpp"

namespace rtype::client {

/**
 * @brief Gives the ship of every pilot @p looks lists a WornLook the first time
 * it sees it, then keeps it equal to the look of that pilot at each call. A
 * ship whose pilot @p looks does not list wears nothing.
 *
 * Called once per frame, before the entities are painted; it allocates only
 * when it gives a ship its WornLook.
 */
void dressPilotShips(engine::ComponentRegistry& components,
                     std::span<const network::PlayerLook> looks);

}  // namespace rtype::client
