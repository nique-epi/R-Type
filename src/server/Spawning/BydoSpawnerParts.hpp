#pragma once

#include <cstdint>
#include <functional>
#include "EntitySpawnSender.hpp"
#include "WavePlan.hpp"

namespace rtype::game {

class World;

}  // namespace rtype::game

namespace rtype::server {

class PlayerRoster;

/** @brief What a BydoSpawner is built from. */
struct BydoSpawnerParts {
  /** @brief The world the Bydos are created in; it must outlive the spawner. */
  std::reference_wrapper<game::World> world;
  /** @brief The players told about each Bydo; it must outlive the spawner. */
  std::reference_wrapper<PlayerRoster> roster;
  /** @brief What is called for each datagram to send. Must not be empty. */
  SendDatagram send;
  /** @brief Where the random draws of the heights start; logged at info level
   * when the spawner is built. */
  std::uint32_t seed{};
  /** @brief The waves of Bydos: their curve must be playable (see
   * game::isPlayable) and their enemies must name at least one kind. */
  WavePlan waves;
};

}  // namespace rtype::server
