#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "BossArrival.hpp"
#include "BossStage.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "EventBus.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world holding the warlord and the minion of the stage tests, and
 * the boss stage of a level whose boss is the warlord; the datagrams it sends
 * and the arrivals it announces are recorded. Nothing moves the bodies.
 */
class BossStageFixture : public ::testing::Test {
 protected:
  BossStageFixture();

  /** @brief Builds the stage again, its level ending with @p boss. */
  void buildStage(std::optional<std::string> boss);

  /** @brief Runs @p count ticks of the stage, the last wave being over or
   * not as @p lastWaveFinished says. */
  void runTicks(std::size_t count, bool lastWaveFinished);

  /** @brief Connects @p count players, who are then told about the boss. */
  void connectPlayers(std::size_t count);

  /** @returns The BossArrival announced so far, once delivered. */
  [[nodiscard]] std::vector<game::BossArrival> arrivals();

  /** @returns Where each boss of the world is. */
  [[nodiscard]] std::vector<game::Position> bosses();

  /** @returns The EntitySpawned sent to the player connected @p index-th. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @brief Makes a minion appear. */
  void addMinion();

  /** @brief Destroys the bosses of the world, as if they were killed. */
  void destroyTheBosses();

  /** @returns Whether the stage says the level is cleared. */
  [[nodiscard]] bool cleared() const;

  /** @returns The number of the sheet of the warlord. */
  [[nodiscard]] std::uint8_t warlordNumber() const;

 private:
  game::EntitySheets sheets_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  engine::EventBus events_;
  std::vector<game::BossArrival> arrivals_;
  std::unique_ptr<BossStage> stage_;
};

}  // namespace rtype::server::testing
