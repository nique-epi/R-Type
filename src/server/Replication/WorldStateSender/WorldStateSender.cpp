#include "WorldStateSender.hpp"
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "NetworkIdentity.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ReplicationConstants.hpp"
#include "ServerConstants.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::server {

namespace {

static_assert(game::PLAYFIELD_WIDTH <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE &&
                  game::PLAYFIELD_HEIGHT <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE,
              "a larger playfield no longer fits a wire coordinate");

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
