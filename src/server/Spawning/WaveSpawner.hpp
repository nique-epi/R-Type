#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <random>
#include <vector>
#include "EntitySheet.hpp"
#include "EntitySpawnSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WaveDirector.hpp"
#include "WaveSpawnerParts.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Makes enemies appear in a world, in waves, and tells the players about
 * each one.
 *
 * A game::WaveDirector says when, on the curve of the wave plan: after a first
 * pause, the enemies of the first wave appear one by one; the next wave, larger
 * and faster, comes a pause after a tick that finds no enemy left in the world,
 * killed or gone on the left. Each tick, the spawner counts the entities of
 * type Enemy in the world and hands that count to the director. When the
 * director moves on to a wave, at the start of the pause before it, the
 * spawner logs the number of the wave, its enemies and the time between two of
 * them at info level (`wave 2: 4 Bydos, one every 1750 ms`). The level holds
 * the waves of the curve: once the last of them is over, no wave follows.
 *
 * The enemies take in turn the sheets the enemies of the plan name, the cycle
 * carrying on from one wave to the next: with `drifter, weaver`, a Drifter,
 * which goes straight, then a Weaver, which weaves up and down, and so on (see
 * game::World::spawn). An enemy appears off screen past the right edge, at a
 * height drawn from a random generator (see game::waveEntryPosition); the
 * sheet uses no random draw, so a seed still gives the same heights. The
 * generator starts from the seed of the parts, which is logged, so the same
 * seed gives the same heights in the same order. Every connected player is sent
 * an EntitySpawned for the enemy, as the Entities section of the protocol page
 * describes. When every network id was given, the enemy is skipped and a
 * warning is logged.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class WaveSpawner {
 public:
  /**
   * @param parts The world, the players, how to send, the seed, which is
   * logged at info level, the wave plan, and the sheets its enemies name.
   * @throws game::InvalidWaveCurveException when the curve of the plan is not
   * playable, EmptyEnemyCycleException when the plan names no enemy, and
   * game::UnknownEntityTypeException when it names a sheet @p parts does not
   * have.
   */
  explicit WaveSpawner(WaveSpawnerParts parts);

  /**
   * @brief Lets one simulation tick of time pass: the enemies the wave director
   * says are due appear.
   */
  void onTick();

  /**
   * @returns Whether the last wave of the level is over: every one of its
   * enemies appeared and a tick found none in the world. It stays true and no
   * enemy appears any more. The game ends the level on it, or on more than it
   * once a boss comes after the last wave.
   */
  [[nodiscard]] bool lastWaveFinished() const;

 private:
  void spawnOne();
  [[nodiscard]] game::Position nextPosition();
  [[nodiscard]] const game::EntitySheet& nextEnemy() const;
  [[nodiscard]] std::optional<std::uint32_t> createEnemy(
      const game::EntitySheet& sheet, game::Position position);
  void announce(const game::EntitySheet& sheet, std::uint32_t networkId,
                game::Position position);
  [[nodiscard]] std::size_t enemiesInPlay();
  void logCurrentWave();

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::mt19937 random_;
  std::vector<std::reference_wrapper<const game::EntitySheet>> enemies_;
  std::size_t spawnedCount_{0};
  game::WaveDirector waveDirector_;
  std::size_t loggedWaveNumber_{0};
};

}  // namespace rtype::server
