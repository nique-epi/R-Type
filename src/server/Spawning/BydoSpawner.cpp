#include "BydoSpawner.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "BydoSpawnPosition.hpp"
#include "BydoSpawnerConstants.hpp"
#include "BydoSpawnerParts.hpp"
#include "BydoWave.hpp"
#include "ComponentRegistry.hpp"
#include "ConnectedPlayer.hpp"
#include "EngineException.hpp"
#include "Entity.hpp"
#include "EntitySheet.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawnSender.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ServerException.hpp"
#include "TimeConstants.hpp"
#include "WavePlan.hpp"
#include "WireEntityTypeMapping.hpp"
#include "World.hpp"

namespace rtype::server {

namespace {

/**
 * @returns The sheets of @p types the names of @p waves name, in their order.
 * @throws EmptyEnemyCycleException when @p waves names none, and
 * game::UnknownEntityTypeException for a name of no sheet.
 */
std::vector<std::reference_wrapper<const game::EntitySheet>> enemySheetsOf(
    const WavePlan& waves, const game::EntitySheets& types) {
  if (waves.enemies.empty()) {
    throw EmptyEnemyCycleException();
  }
  std::vector<std::reference_wrapper<const game::EntitySheet>> sheets;
  sheets.reserve(waves.enemies.size());
  for (const std::string& name : waves.enemies) {
    sheets.emplace_back(types.named(name));
  }
  return sheets;
}

}  // namespace

BydoSpawner::BydoSpawner(BydoSpawnerParts parts)
    : world_(&parts.world.get()),
      roster_(&parts.roster.get()),
      send_(std::move(parts.send)),
      logger_(std::string{BYDO_SPAWNER_LOGGER_NAME}),
      random_(parts.seed),
      enemies_(enemySheetsOf(parts.waves, parts.types.get())),
      waveDirector_(parts.waves.curve) {
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

bool BydoSpawner::lastWaveFinished() const {
  return waveDirector_.lastWaveFinished();
}

void BydoSpawner::spawnOne() {
  const game::Position position = nextPosition();
  const game::EntitySheet& sheet = nextEnemy();
  const std::optional<std::uint32_t> networkId = createBydo(sheet, position);
  if (!networkId.has_value()) {
    return;
  }
  ++spawnedCount_;
  announce(sheet, *networkId, position);
}

game::Position BydoSpawner::nextPosition() {
  std::uniform_real_distribution<float> unitDraw{0.0F, 1.0F};
  const float highestDraw = std::nextafter(1.0F, 0.0F);
  return game::bydoSpawnPosition(std::min(unitDraw(random_), highestDraw));
}

const game::EntitySheet& BydoSpawner::nextEnemy() const {
  return enemies_[spawnedCount_ % enemies_.size()];
}

std::optional<std::uint32_t> BydoSpawner::createBydo(
    const game::EntitySheet& sheet, game::Position position) {
  try {
    return world_->spawn(sheet, position);
  } catch (const engine::NetworkIdExhaustedException&) {
    logger_.warn("no Bydo appears: every network id was given");
    return std::nullopt;
  }
}

void BydoSpawner::announce(const game::EntitySheet& sheet,
                           std::uint32_t networkId, game::Position position) {
  for (ConnectedPlayer* const player : roster_->connected()) {
    sendEntitySpawned(
        send_, *player,
        network::EntitySpawned{.entityId = networkId,
                               .entityType = toWireEntityType(sheet.side),
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
