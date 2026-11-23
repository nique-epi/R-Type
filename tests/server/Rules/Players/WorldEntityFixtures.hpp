#pragma once

#include <cstdint>
#include "Entity.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/** @returns The entity of @p world that carries @p networkId; a default one,
 * after a failure, when none does. */
[[nodiscard]] engine::Entity entityWithNetworkId(game::World& world,
                                                 std::uint32_t networkId);

}  // namespace rtype::server::testing
