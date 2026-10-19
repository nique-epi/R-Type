#include "BydoSpawner.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <random>
#include <string>
#include <utility>
#include "BydoKind.hpp"
#include "BydoSpawnPosition.hpp"
#include "BydoSpawnerConstants.hpp"
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

BydoSpawner::BydoSpawner(game::World& world, PlayerRoster& roster,
                         SendDatagram send, std::uint32_t seed)
    : world_(&world),
      roster_(&roster),
      send_(std::move(send)),
      logger_(std::string{BYDO_SPAWNER_LOGGER_NAME}),
      random_(seed) {
  logger_.info("Bydo spawning starts from the seed ", seed);
  timers_.scheduleRepeating(BYDO_SPAWN_INTERVAL, [this] { spawnOne(); });
}

void BydoSpawner::onTick() {
  timers_.advance(engine::SIMULATION_TICK_DURATION);
}

void BydoSpawner::spawnOne() {
  std::uniform_real_distribution<float> unitDraw{0.0F, 1.0F};
  const float highestDraw = std::nextafter(1.0F, 0.0F);
  const game::Position position =
      game::bydoSpawnPosition(std::min(unitDraw(random_), highestDraw));
  const game::BydoKind kind =
      spawnedCount_ % 2 == 0 ? game::BydoKind::Drifter : game::BydoKind::Weaver;
  std::uint32_t networkId = 0;
  try {
    networkId = world_->spawnBydo(position, kind);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no Bydo appears: every network id was given");
    return;
  }
  ++spawnedCount_;
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
