#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EventBus.hpp"
#include "ExtraLifeKeeper.hpp"
#include "ExtraLifeWon.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ScoreKeeper.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world whose players each fly a ship, the ScoreKeeper of the game
 * subscribed first, as the game builds it, then an ExtraLifeKeeper, and the
 * extra lives it announces.
 */
class ExtraLifeKeeperFixture : public ::testing::Test {
 protected:
  ExtraLifeKeeperFixture();

  /** @returns The player @p index, seated with a ship and the lives of a new
   * ship. */
  ConnectedPlayer& join(std::size_t index);

  /** @brief Publishes that @p player killed an enemy worth @p points, and
   * delivers it with what it leads to. */
  void kill(const ConnectedPlayer& player, std::uint32_t points);

  [[nodiscard]] game::World& world();
  [[nodiscard]] const std::vector<game::ExtraLifeWon>& wins() const;

 private:
  engine::EventBus events_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_;
  ScoreKeeper scores_;
  ExtraLifeKeeper keeper_;
  std::vector<game::ExtraLifeWon> wins_;
};

}  // namespace rtype::server::testing
