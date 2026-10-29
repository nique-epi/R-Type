#include "WorldStateSender.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "ManeuverKind.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReplicationConstants.hpp"
#include "ServerConstants.hpp"
#include "SessionDatagram.hpp"
#include "ShipMotion.hpp"
#include "WireEntityTypeMapping.hpp"
#include "WorldState.hpp"
#include "WorldStateCodec.hpp"

namespace rtype::server {

namespace {

static_assert(game::PLAYFIELD_WIDTH <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE &&
                  game::PLAYFIELD_HEIGHT <= LARGEST_PLAYFIELD_SIZE_ON_THE_WIRE,
              "a larger playfield no longer fits a wire coordinate");

/** @returns A byte with only the bit at @p position set. */
std::uint8_t bitAt(std::uint8_t position) {
  return static_cast<std::uint8_t>(1U << static_cast<unsigned int>(position));
}

/** @returns The bits of the directions the newest input of @p player holds,
 * where a PlayerState carries them in its motion; 0 before any input. */
std::uint8_t heldDirectionsOf(const ConnectedPlayer& player) {
  if (!player.newestInput.has_value()) {
    return 0;
  }
  return static_cast<std::uint8_t>(player.newestInput->actions &
                                   network::DIRECTION_ACTION_BITS);
}

/** @returns The bit of the motion that tells @p kind; 0 for no maneuver. */
std::uint8_t maneuverBitOf(game::ManeuverKind kind) {
  switch (kind) {
    case game::ManeuverKind::Roll:
      return bitAt(network::ROLLING_MOTION_BIT_POSITION);
    case game::ManeuverKind::Dash:
      return bitAt(network::DASHING_MOTION_BIT_POSITION);
    case game::ManeuverKind::None:
      return 0;
  }
  return 0;
}

/** @returns The maneuver bit of the ship whose network id is @p shipId in
 * @p components; 0 when no ship of that id has a ShipMotion. */
std::uint8_t maneuverBitOfShip(engine::ComponentRegistry& components,
                               std::uint32_t shipId) {
  std::uint8_t bit = 0;
  components.forEach<game::ShipMotion, game::NetworkIdentity>(
      [shipId, &bit](engine::Entity, const game::ShipMotion& motion,
                     const game::NetworkIdentity& identity) {
        if (identity.networkId == shipId) {
          bit = maneuverBitOf(motion.maneuver);
        }
      });
  return bit;
}

}  // namespace

WorldStateSender::WorldStateSender(PlayerRoster& roster, Send send)
    : roster_(&roster),
      send_(std::move(send)),
      logger_(std::string{REPLICATION_LOGGER_NAME}) {}

void WorldStateSender::onTick(std::uint32_t tick,
                              engine::ComponentRegistry& components) {
  if (tick % TICKS_PER_WORLD_STATE != 0) {
    return;
  }
  const ConnectedPlayers players = roster_->connected();
  if (players.empty()) {
    return;
  }
  std::size_t leftOutCount = 0;
  network::WorldState state = capture(tick, components, leftOutCount);
  state.players = playerStatesOf(players, components);
  reportLeftOut(leftOutCount);
  const std::vector<std::vector<std::byte>> payloads =
      network::WorldStateCodec::encode(state);
  for (ConnectedPlayer* const player : players) {
    for (const std::vector<std::byte>& payload : payloads) {
      send_(sessionDatagram(*player, network::WorldState::TYPE, payload));
    }
  }
}

std::vector<network::PlayerState> WorldStateSender::playerStatesOf(
    const ConnectedPlayers& players, engine::ComponentRegistry& components) {
  std::vector<network::PlayerState> states;
  states.reserve(players.size());
  for (const ConnectedPlayer* const player : players) {
    states.push_back(network::PlayerState{
        .playerNumber = player->playerNumber,
        .shipEntityId = player->shipEntityId,
        .lives = player->lives,
        .score = player->score,
        .invulnerable = player->invulnerableTicksLeft > 0,
        .motion = static_cast<std::uint8_t>(
            heldDirectionsOf(*player) |
            maneuverBitOfShip(components, player->shipEntityId))});
  }
  return states;
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
