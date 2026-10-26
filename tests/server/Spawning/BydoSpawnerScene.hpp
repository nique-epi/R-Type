#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "BydoSpawner.hpp"
#include "EntitySpawned.hpp"
#include "MovementPattern.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "WavePlan.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world and a roster a Bydo spawner works on, the datagrams it sends
 * recorded.
 */
class BydoSpawnerScene {
 public:
  /** @brief Connects the player at @p index, who joins as player index + 1. */
  void connect(std::size_t index);

  /**
   * @returns A spawner on this world and roster, started from @p seed, on
   * testWavePlan(), whose datagrams this scene records.
   */
  [[nodiscard]] BydoSpawner makeSpawner(std::uint32_t seed);

  /** @returns The same, on @p waves. */
  [[nodiscard]] BydoSpawner makeSpawnerWith(std::uint32_t seed, WavePlan waves);

  /** @brief Runs @p count ticks of @p spawner. */
  static void runTicks(BydoSpawner& spawner, std::size_t count);

  /**
   * @returns The heights of the Bydos announced to the player at @p index,
   * oldest first.
   */
  [[nodiscard]] std::vector<float> heightsSentTo(std::size_t index) const;

  /** @returns Every EntitySpawned sent, whoever it went to, oldest first. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSent() const;

  /**
   * @returns The heights announced to FIRST_PLAYER by a spawner started from
   * @p seed on a scene of its own, in the first @p ticks of a game.
   */
  [[nodiscard]] static std::vector<float> heightsWithin(std::uint32_t seed,
                                                        std::size_t ticks);

  /**
   * @brief Destroys every entity of type Enemy, as if the Bydos were all
   * killed.
   */
  void clearTheSky();

  /** @returns The shapes of the patterns of the Bydos in the world. */
  [[nodiscard]] std::vector<game::PatternShape> patternsOfTheBydos();

  [[nodiscard]] game::World& world();

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
};

}  // namespace rtype::server::testing
