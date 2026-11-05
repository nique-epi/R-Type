#pragma once

#include <cstdint>
#include <optional>
#include "Entity.hpp"
#include "NetworkIdentity.hpp"
#include "World.hpp"

namespace rtype::game::testing {

/**
 * @returns The Component of the entity of @p world that carries @p networkId,
 * or nothing when no such entity has one.
 */
template <typename Component>
std::optional<Component> componentOf(World& world, std::uint32_t networkId) {
  std::optional<Component> found;
  world.components().forEach<NetworkIdentity, Component>(
      [&found, networkId](engine::Entity /*entity*/,
                          const NetworkIdentity& identity,
                          const Component& component) {
        if (identity.networkId == networkId) {
          found = component;
        }
      });
  return found;
}

}  // namespace rtype::game::testing
