#pragma once

#include <cstdint>
#include <optional>
#include <random>
#include "BydoKind.hpp"
#include "BydoSpawnerParts.hpp"
#include "EntitySpawnSender.hpp"
#include "Logger.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "TimerScheduler.hpp"
#include "World.hpp"

namespace rtype::server {

/**
 * @brief Makes Bydos appear in a world, one every BYDO_SPAWN_INTERVAL of
 * simulation time, and tells the players about each one.
 *
 * A Bydo appears off screen past the right edge, at a height drawn from a
 * random generator (see game::bydoSpawnPosition). The kinds of Bydo alternate,
 * starting with the Drifter, which goes straight; the Weaver follows a wave.
 * The kind uses no random draw, so a seed still gives the same heights.
 * The generator starts from the seed of the parts, which is logged, so the
 * same seed gives the same heights in the same order. Every connected player is
 * sent an EntitySpawned for the Bydo, as the Entities section of the protocol
 * page describes. When every network id was given, the Bydo of that interval is
 * skipped and a warning is logged.
 *
 * Not thread-safe: use it from the simulation thread only.
 */
class BydoSpawner {
 public:
  /**
   * @param parts The world, the players, how to send, and the seed, which is
   * logged at info level.
   */
  explicit BydoSpawner(BydoSpawnerParts parts);

  /**
   * @brief Lets one simulation tick of time pass: a Bydo appears for each
   * BYDO_SPAWN_INTERVAL that is complete.
   */
  void onTick();

 private:
  void spawnOne();
  [[nodiscard]] game::Position nextPosition();
  [[nodiscard]] game::BydoKind nextKind() const;
  [[nodiscard]] std::optional<std::uint32_t> createBydo(game::Position position,
                                                        game::BydoKind kind);
  void announce(std::uint32_t networkId, game::Position position);

  game::World* world_;
  PlayerRoster* roster_;
  SendDatagram send_;
  logging::Logger logger_;
  std::mt19937 random_;
  std::uint32_t spawnedCount_{0};
  engine::TimerScheduler timers_;
};

}  // namespace rtype::server
