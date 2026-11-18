#include "EntityOrdersFixture.hpp"
#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include "BehaviourScripts.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityOrdersParts.hpp"
#include "EntitySpawned.hpp"
#include "Health.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "SentDatagrams.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShippedEntitySheets.hpp"
#include "SpawnOptions.hpp"
#include "World.hpp"

namespace rtype::server::testing {

namespace {

/** @returns The @p Component of the body of @p networkId in @p world;
 * nothing when no body has that id or it has no such component. */
template <typename Component>
std::optional<Component> componentOf(game::World& world,
                                     std::uint32_t networkId) {
  std::optional<Component> found;
  world.components().forEach<game::NetworkIdentity, Component>(
      [&found, networkId](engine::Entity, const game::NetworkIdentity& identity,
                          const Component& component) {
        if (identity.networkId == networkId) {
          found = component;
        }
      });
  return found;
}

}  // namespace

EntityOrdersFixture::EntityOrdersFixture() : bodies_(partsFor(nullptr)) {
  static_cast<void>(events_.subscribe<game::EntityDestruction>(
      [this](const game::EntityDestruction& removal) {
        removals_.push_back(removal);
      }));
}

void EntityOrdersFixture::connectPlayers(std::size_t count) {
  for (std::size_t index = 0; index < count; ++index) {
    EXPECT_NE(roster_.add(playerEndpoint(index),
                          std::string{PLAYER_NICKNAME} + std::to_string(index),
                          SESSION_TOKEN + index),
              nullptr);
  }
}

std::uint32_t EntityOrdersFixture::spawnByScript(std::string_view sheet,
                                                 game::Position position,
                                                 game::SpawnOptions options) {
  return bodies_.spawn(sheet, position, options);
}

void EntityOrdersFixture::removeByScript(std::uint32_t networkId) {
  bodies_.destroy(networkId);
}

void EntityOrdersFixture::endTick() {
  bodies_.onTick();
  events_.dispatch();
}

std::uint32_t EntityOrdersFixture::placeDirectly(std::string_view sheet,
                                                 game::Position position) {
  return world_.spawn(game::testing::shippedEntitySheets().named(sheet),
                      position);
}

std::optional<game::Position> EntityOrdersFixture::positionOf(
    std::uint32_t networkId) {
  return componentOf<game::Position>(world_, networkId);
}

std::optional<int> EntityOrdersFixture::healthOf(std::uint32_t networkId) {
  const std::optional<game::Health> health =
      componentOf<game::Health>(world_, networkId);
  if (!health.has_value()) {
    return std::nullopt;
  }
  return health->points;
}

std::vector<network::EntitySpawned> EntityOrdersFixture::spawnsSentTo(
    std::size_t index) const {
  return onlyMessagesIn<network::EntitySpawned>(
      sentTo(sent_, playerEndpoint(index)));
}

EntityOrdersParts EntityOrdersFixture::partsFor(
    game::BehaviourScripts* scripts) {
  return EntityOrdersParts{.world = world_,
                           .roster = roster_,
                           .send =
                               [this](network::OutgoingDatagram datagram) {
                                 sent_.push_back(std::move(datagram));
                               },
                           .types = game::testing::shippedEntitySheets(),
                           .events = events_,
                           .scripts = scripts};
}

const std::vector<game::EntityDestruction>& EntityOrdersFixture::removals()
    const {
  return removals_;
}

}  // namespace rtype::server::testing
