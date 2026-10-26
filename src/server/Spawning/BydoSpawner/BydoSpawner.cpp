#include "BydoSpawner.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include "BydoKind.hpp"
#include "BydoSpawnPosition.hpp"
#include "BydoSpawnerConstants.hpp"
#include "BydoSpawnerParts.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

BydoSpawner::BydoSpawner(BydoSpawnerParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      logger_(std::string{BYDO_SPAWNER_LOGGER_NAME}),
      random_(parts.seed) {
  logger_.info("Bydo spawning starts from the seed ", parts.seed);
  timers_.scheduleRepeating(BYDO_SPAWN_INTERVAL, [this] { spawnOne(); });
}

void BydoSpawner::onTick() {
  timers_.advance(engine::SIMULATION_TICK_DURATION);
}

void BydoSpawner::spawnOne() {
  const game::Position position = nextPosition();
  const std::optional<std::uint32_t> networkId =
      createBydo(position, nextKind());
  if (!networkId.has_value()) {
    return;
  }
  ++spawnedCount_;
  announce(*networkId, position);
}

game::Position BydoSpawner::nextPosition() {
  std::uniform_real_distribution<float> unitDraw{0.0F, 1.0F};
  const float highestDraw = std::nextafter(1.0F, 0.0F);
  return game::bydoSpawnPosition(std::min(unitDraw(random_), highestDraw));
}

game::BydoKind BydoSpawner::nextKind() const {
  return spawnedCount_ % 2 == 0 ? game::BydoKind::Drifter
                                : game::BydoKind::Weaver;
}

std::optional<std::uint32_t> BydoSpawner::createBydo(game::Position position,
                                                     game::BydoKind kind) {
  try {
    return world_->spawnBydo(position, kind);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no Bydo appears: every network id was given");
    return std::nullopt;
  }
}

void BydoSpawner::announce(std::uint32_t networkId, game::Position position) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(
        send_, *player,
        network::EntitySpawned{.entityId = networkId,
                               .entityType = network::WireEntityType::Bydo,
                               .x = position.x,
                               .y = position.y});
  }
}

}  // namespace rtype::server
