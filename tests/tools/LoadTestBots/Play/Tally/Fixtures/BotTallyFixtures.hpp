#pragma once

#include <cstdint>
#include "EntityMessage.hpp"

namespace rtype::tools::testing {

/** @returns A world state of @p tick, with no player and no entity. */
[[nodiscard]] client::EntityMessage worldStateAt(std::uint32_t tick);

/** @returns The end of a game, with no score. */
[[nodiscard]] client::EntityMessage gameEnded();

}  // namespace rtype::tools::testing
