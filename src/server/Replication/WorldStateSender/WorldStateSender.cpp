#include "WorldStateSender.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "Position.hpp"
#include "ServerConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"
// NOLINTNEXTLINE(misc-include-cleaner)
#include "WorldStateSenderConstants.hpp"

namespace rtype::server {

namespace {

network::WireEntityType toWireEntityType(game::EntityType type) {
  switch (type) {
    case game::EntityType::Player:
      return network::WireEntityType::PlayerShip;
    case game::EntityType::Enemy:
      return network::WireEntityType::Bydo;
    case game::EntityType::PlayerMissile:
      return network::WireEntityType::PlayerMissile;
    case game::EntityType::EnemyMissile:
      return network::WireEntityType::EnemyMissile;
  }
  return network::WireEntityType::PlayerShip;
}

}  // namespace

WorldStateSender::WorldStateSender(Send send) : send_(std::move(send)) {}

void WorldStateSender::onTick(engine::ComponentRegistry& components,
                              std::span<const network::Endpoint> recipients) {
  ++tick_;
  if (tick_ % TICKS_PER_WORLD_STATE != 0 || recipients.empty()) {
    return;
  }
  const std::vector<std::vector<std::byte>> payloads =
      network::WorldStateCodec::encode(capture(components));
  for (const network::Endpoint& recipient : recipients) {
    for (const std::vector<std::byte>& payload : payloads) {
      send_(network::OutgoingDatagram{.destination = recipient,
                                      .payload = payload});
    }
  }
}

std::uint32_t WorldStateSender::currentTick() const { return tick_; }

network::WorldState WorldStateSender::capture(
    engine::ComponentRegistry& components) const {
  network::WorldState state{.tick = tick_, .players = {}, .entities = {}};
  components.forEach<game::NetworkIdentity, game::Position>(
      [&components, &state](engine::Entity entity,
                            const game::NetworkIdentity& identity,
                            const game::Position& position) {
        const game::EntityType* type = components.get<game::EntityType>(entity);
        if (type == nullptr) {
          return;
        }
        state.entities.push_back(
            network::EntityState{.entityId = identity.networkId,
                                 .entityType = toWireEntityType(*type),
                                 .x = position.x,
                                 .y = position.y});
      });
  return state;
}

}  // namespace rtype::server
