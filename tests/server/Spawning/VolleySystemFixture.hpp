#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "ActionKey.hpp"
#include "EntitySheets.hpp"
#include "EntitySpawned.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "VolleySystem.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/**
 * @brief A world holding the brooder and the minion of the volley tests, and
 * the volley system that fires its volleys; the datagrams it sends are
 * recorded. Nothing moves the bodies and no behaviour steers: a test starts
 * an action by writing it in the Intent of the brooders.
 */
class VolleySystemFixture : public ::testing::Test {
 protected:
  VolleySystemFixture();

  /** @brief Makes a brooder appear at BROODER_CENTER. */
  void addBrooder();

  /** @brief Connects @p count players, who are then told about each body. */
  void connectPlayers(std::size_t count);

  /** @brief Writes @p action as the action every brooder starts this tick. */
  void startAction(game::ActionKey action);

  /** @brief Runs one tick of the volley system. */
  void runTick();

  /** @returns Where each minion of the world is, from the highest to the
   * lowest. */
  [[nodiscard]] std::vector<game::Position> minions();

  /** @returns How many enemies the world holds. */
  [[nodiscard]] std::size_t enemies();

  /** @returns The EntitySpawned sent to the player connected @p index-th. */
  [[nodiscard]] std::vector<network::EntitySpawned> spawnsSentTo(
      std::size_t index) const;

  /** @returns The number of the sheet of the minions. */
  [[nodiscard]] std::uint8_t minionNumber() const;

 private:
  game::EntitySheets sheets_;
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  VolleySystem volleys_;
};

}  // namespace rtype::server::testing
