#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
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
      [&found, networkId](engine::Entity, const NetworkIdentity& identity,
                          const Component& component) {
        if (identity.networkId == networkId) {
          found = component;
        }
      });
  return found;
}

/**
 * @returns The name of the behaviour that pilots the entity of @p world
 * carrying @p networkId, or nothing when it has no Pilot.
 */
[[nodiscard]] std::optional<std::string_view> behaviourNameOf(
    World& world, std::uint32_t networkId);

/**
 * @returns The settings of the Pilot of the entity of @p world carrying
 * @p networkId, in the order its behaviour declares them; empty when it has no
 * Pilot.
 */
[[nodiscard]] std::vector<std::int64_t> pilotSettingsOf(
    World& world, std::uint32_t networkId);

}  // namespace rtype::game::testing
