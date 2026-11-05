#pragma once

#include <gtest/gtest.h>
#include <cstddef>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "EntityTypesTestConstants.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "ShipSpawnAnnouncerParts.hpp"
#include "ShippedEntitySheets.hpp"
#include "World.hpp"

namespace rtype::server::testing {

/** @brief An announcer whose datagrams are recorded instead of sent. */
class ShipSpawnAnnouncerFixture : public ::testing::Test {
 protected:
  /** @brief Connects the player number @p index + 1 and announces it. */
  ConnectedPlayer& join(std::size_t index);

  /** @returns The datagrams sent to @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const;

  [[nodiscard]] game::World& world();

  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const;

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_{ShipSpawnAnnouncerParts{
      .world = world_,
      .roster = roster_,
      .send =
          [this](network::OutgoingDatagram datagram) {
            sent_.push_back(std::move(datagram));
          },
      .ship = game::testing::shippedEntitySheets().named(
          game::testing::SHIP_SHEET)}};
};

}  // namespace rtype::server::testing
