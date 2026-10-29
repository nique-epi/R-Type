#include "WorldStateSender.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "ConnectedPlayers.hpp"
#include "Entity.hpp"
#include "EntityType.hpp"
#include "Intent.hpp"
#include "Invulnerable.hpp"
#include "Lives.hpp"
#include "ManeuverKind.hpp"
#include "NetworkIdentity.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldConstants.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ReplicationConstants.hpp"
#include "ServerConstants.hpp"
#include "ServerException.hpp"
#include "SessionDatagram.hpp"
#include "ShipDirections.hpp"
#include "ShipMotion.hpp"
#include "TimeConstants.hpp"
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

/** @returns The bits of @p directions, where a PlayerState carries them in its
 * motion. */
std::uint8_t directionBitsOf(const game::ShipDirections& directions) {
  std::uint8_t bits = 0;
  if (directions.up) {
    bits |= bitAt(network::UP_ACTION_BIT_POSITION);
  }
  if (directions.down) {
    bits |= bitAt(network::DOWN_ACTION_BIT_POSITION);
  }
  if (directions.left) {
    bits |= bitAt(network::LEFT_ACTION_BIT_POSITION);
  }
  if (directions.right) {
    bits |= bitAt(network::RIGHT_ACTION_BIT_POSITION);
  }
  return bits;
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

/** @brief What a PlayerState reads off the ship of a player. */
struct ShipReading {
  std::optional<std::uint8_t> lives;
  bool invulnerable{false};
  std::uint8_t motion{0};
};

/** @returns What the ship whose network id is @p shipId in @p components tells
 * a PlayerState: its lives and protection, and the bits of the directions its
 * Intent holds and of its maneuver. Nothing is read when no ship has that id
 * and a ShipMotion. */
ShipReading readShip(engine::ComponentRegistry& components,
                     std::uint32_t shipId) {
  ShipReading reading;
  components.forEach<game::ShipMotion, game::NetworkIdentity>(
      [&components, shipId, &reading](engine::Entity ship,
                                      const game::ShipMotion& motion,
                                      const game::NetworkIdentity& identity) {
        if (identity.networkId != shipId) {
          return;
        }
        reading.motion = maneuverBitOf(motion.maneuver);
        if (const game::Intent* const intent =
                components.get<game::Intent>(ship)) {
          reading.motion |= directionBitsOf(intent->directions);
        }
        if (const game::Lives* const lives =
                components.get<game::Lives>(ship)) {
          reading.lives = lives->count;
        }
        if (const game::Invulnerable* const protection =
                components.get<game::Invulnerable>(ship)) {
          reading.invulnerable = protection->ticksLeft > 0;
        }
      });
  return reading;
}

}  // namespace

WorldStateSender::WorldStateSender(PlayerRoster& roster, Send send,
                                   std::size_t statesPerSecond)
    : roster_(&roster),
      send_(std::move(send)),
      statesPerSecond_(statesPerSecond),
      logger_(std::string{REPLICATION_LOGGER_NAME}) {
  if (statesPerSecond < LOWEST_WORLD_STATES_PER_SECOND ||
      statesPerSecond > HIGHEST_WORLD_STATES_PER_SECOND) {
    throw InvalidStateRateException(statesPerSecond);
  }
}

bool WorldStateSender::carriesState(std::uint32_t tick) const {
  const std::uint64_t shareOfSecond = static_cast<std::uint64_t>(tick) *
                                      statesPerSecond_ %
                                      engine::SIMULATION_TICKS_PER_SECOND;
  return shareOfSecond < statesPerSecond_;
}

void WorldStateSender::onTick(std::uint32_t tick,
                              engine::ComponentRegistry& components) {
  if (!carriesState(tick)) {
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
    const ShipReading ship = readShip(components, player->shipEntityId);
    states.push_back(network::PlayerState{
        .playerNumber = player->playerNumber,
        .shipEntityId = player->shipEntityId,
        .lives = ship.lives.value_or(player->livesWithoutShip),
        .score = player->score,
        .invulnerable = ship.invulnerable,
        .motion = ship.motion});
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
