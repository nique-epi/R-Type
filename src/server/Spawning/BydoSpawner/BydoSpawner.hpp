#pragma once

#include <cstdint>
#include <random>
#include "EntitySpawnSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "TimerScheduler.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Makes Bydos appear in a world, one every BYDO_SPAWN_INTERVAL of
 * simulation time, and tells the players about each one.
 *
 * A Bydo appears off screen past the right edge, at a height drawn from a
 * random generator (see game::bydoSpawnPosition). The generator starts from
 * the seed given at construction, which is logged, so the same seed gives the
 * same heights in the same order. Every connected player is sent an
 * EntitySpawned for the Bydo, as the Entities section of the protocol page
 * describes. When every network id was given, the Bydo of that interval is
 * skipped and a warning is logged.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class BydoSpawner {
 public:
  /**
   * @param world The world the Bydos are created in; it must outlive the
   * spawner.
   * @param roster The players of the game; it must outlive the spawner.
   * @param send Must not be empty.
   * @param seed Starts the random generator; logged at info level.
   */
  BydoSpawner(game::World& world, PlayerRoster& roster, SendDatagram send,
              std::uint32_t seed);

  /**
   * @brief Lets one simulation tick of time pass: a Bydo appears for each
   * BYDO_SPAWN_INTERVAL that is complete.
   */
  void onTick();

 private:
  void spawnOne();

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::mt19937 random_;
  engine::TimerScheduler timers_;
};

}  // namespace rtype::server
