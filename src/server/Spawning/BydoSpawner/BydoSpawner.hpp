#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <random>
#include <vector>
#include "BydoKind.hpp"
#include "BydoSpawnerParts.hpp"
#include "EntitySpawnSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "WaveDirector.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Makes Bydos appear in a world, in waves, and tells the players about
 * each one.
 *
 * A game::WaveDirector says when, on the curve of the wave plan: after a first
 * pause, the Bydos of the first wave appear one by one; the next wave, larger
 * and faster, comes a pause after a tick that finds no Bydo left in the world,
 * killed or gone on the left. Each tick, the spawner counts the entities of
 * type Enemy in the world and hands that count to the director. When the
 * director moves on to a wave, at the start of the pause before it, the
 * spawner logs the number of the wave, its Bydos and the time between two of
 * them at info level (`wave 2: 4 Bydos, one every 1750 ms`).
 *
 * The Bydos take the kinds the enemies of the plan list, in turn, the cycle
 * carrying on from one wave to the next: with `drifter, weaver`, a Drifter,
 * which goes straight, then a Weaver, which weaves up and down, and so on. A
 * Bydo appears off screen past the right edge, at a height drawn from a random
 * generator (see game::bydoSpawnPosition); the kind uses no random draw, so a
 * seed still gives the same heights. The generator starts from the seed of the
 * parts, which is logged, so the same seed gives the same heights in the same
 * order. Every connected player is sent an EntitySpawned for the Bydo, as the
 * Entities section of the protocol page describes. When every network id was
 * given, the Bydo is skipped and a warning is logged.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class BydoSpawner {
 public:
  /**
   * @param parts The world, the players, how to send, the seed, which is
   * logged at info level, and the wave plan.
   * @throws game::InvalidWaveCurveException when the curve of the plan is not
   * playable, and EmptyEnemyCycleException when the plan names no enemy.
   */
  explicit BydoSpawner(BydoSpawnerParts parts);

  /**
   * @brief Lets one simulation tick of time pass: the Bydos the wave director
   * says are due appear.
   */
  void onTick();

 private:
  void spawnOne();
  [[nodiscard]] game::Position nextPosition();
  [[nodiscard]] game::BydoKind nextKind() const;
  [[nodiscard]] std::optional<std::uint32_t> createBydo(game::Position position,
                                                        game::BydoKind kind);
  void announce(std::uint32_t networkId, game::Position position);
  [[nodiscard]] std::size_t bydosInPlay();
  void logCurrentWave();

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::mt19937 random_;
  std::vector<game::BydoKind> enemies_;
  std::size_t spawnedCount_{0};
  game::WaveDirector waveDirector_;
  std::size_t loggedWaveNumber_{0};
};

}  // namespace rtype::server
