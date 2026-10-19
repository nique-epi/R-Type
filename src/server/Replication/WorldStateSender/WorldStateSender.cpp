#include "WorldStateSender.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
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
#include "WireEntityTypeMapping.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::server {

namespace {

static_assert(game::PLAYFIELD_WIDTH <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE &&
                  game::PLAYFIELD_HEIGHT <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE,
              "a larger playfield no longer fits a wire coordinate");

}  // namespace

WorldStateSender::WorldStateSender(Send send)
    : send_(std::move(send)), logger_(std::string{REPLICATION_LOGGER_NAME}) {}

void WorldStateSender::onTick(std::uint32_t tick,
                              engine::ComponentRegistry& components,
                              std::span<const network::Endpoint> recipients) {
  if (tick % TICKS_PER_WORLD_STATE != 0 || recipients.empty()) {
    return;
  }
  std::size_t leftOutCount = 0;
  const network::WorldState state = capture(tick, components, leftOutCount);
  reportLeftOut(leftOutCount);
  const std::vector<std::vector<std::byte>> payloads =
      network::WorldStateCodec::encode(state);
  for (const network::Endpoint& recipient : recipients) {
    for (const std::vector<std::byte>& payload : payloads) {
      send_(network::OutgoingDatagram{.destination = recipient,
                                      .payload = payload});
    }
  }
}

network::WorldState WorldStateSender::capture(
    std::uint32_t tick, engine::ComponentRegistry& components,
    std::size_t& leftOutCount) {
  network::WorldState state{.tick = tick, .players = {}, .entities = {}};
  components.forEach<game::NetworkIdentity, game::Position>(
      [&components, &state, &leftOutCount](
          engine::Entity entity, const game::NetworkIdentity& identity,
          const game::Position& position) {
        const game::EntityType* type = components.get<game::EntityType>(entity);
        if (type == nullptr) {
          return;
        }
        if (!std::isfinite(position.x) || !std::isfinite(position.y)) {
          ++leftOutCount;
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

void WorldStateSender::reportLeftOut(std::size_t leftOutCount) {
  if (leftOutCount != reportedLeftOutCount_ && leftOutCount != 0) {
    logger_.warn(leftOutCount,
                 " entities left out of the world state: their position is "
                 "not a finite number");
  }
  reportedLeftOutCount_ = leftOutCount;
}

}  // namespace rtype::server
