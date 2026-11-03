#include "BydoSpawner.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include "BydoKind.hpp"
#include "BydoSpawnPosition.hpp"
#include "BydoSpawnerConstants.hpp"
#include "BydoSpawnerParts.hpp"
#include "BydoWave.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ServerException.hpp"
#include "TimeConstants.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {

BydoSpawner::BydoSpawner(BydoSpawnerParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      logger_(std::string{BYDO_SPAWNER_LOGGER_NAME}),
      random_(parts.seed),
      enemies_(std::move(parts.waves.enemies)),
      waveDirector_(parts.waves.curve) {
  if (enemies_.empty()) {
    throw EmptyEnemyCycleException();
  }
  logger_.info("Bydo spawning starts from the seed ", parts.seed);
}

void BydoSpawner::onTick() {
  const std::size_t bydosDue =
      waveDirector_.advance(engine::SIMULATION_TICK_DURATION, bydosInPlay());
  if (waveDirector_.waveNumber() != loggedWaveNumber_) {
    logCurrentWave();
  }
  for (std::size_t bydo = 0; bydo < bydosDue; ++bydo) {
    spawnOne();
  }
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
  return enemies_[spawnedCount_ % enemies_.size()];
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

std::size_t BydoSpawner::bydosInPlay() {
  std::size_t bydos = 0;
  world_->components().forEach<game::EntityType, game::EntityType>(
      [&bydos](engine::Entity, const game::EntityType& type,
               const game::EntityType&) {
        if (type == game::EntityType::Enemy) {
          ++bydos;
        }
      });
  return bydos;
}

void BydoSpawner::logCurrentWave() {
  loggedWaveNumber_ = waveDirector_.waveNumber();
  const game::BydoWave wave = waveDirector_.currentWave();
  logger_.info(
      "wave ", loggedWaveNumber_, ": ", wave.bydoCount, " Bydos, one every ",
      std::chrono::duration_cast<std::chrono::milliseconds>(wave.spawnInterval)
          .count(),
      " ms");
}

}  // namespace rtype::server
