#pragma once

#include <cstddef>
#include <cstdint>
#include "EntityMessage.hpp"

namespace rtype::tools::testing {

/** @returns A world state of @p tick, with no player and no entity. */
[[nodiscard]] client::EntityMessage worldStateAt(std::uint32_t tick);

/** @returns A world state of @p tick, with no player and @p entityCount
 * entities, each at its default: the tally only counts them. */
[[nodiscard]] client::EntityMessage worldStateListing(std::uint32_t tick,
                                                      std::size_t entityCount);

/** @returns The end of a game, with no score. */
[[nodiscard]] client::EntityMessage gameEnded();

}  // namespace rtype::tools::testing
