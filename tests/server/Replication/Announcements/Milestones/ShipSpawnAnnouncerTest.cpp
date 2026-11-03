#include <gtest/gtest.h>
#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "ConnectedPlayer.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "EntityIdConstants.hpp"
#include "EntitySpawned.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "OutgoingDatagram.hpp"
#include "PacketHeader.hpp"
#include "PlayerRoster.hpp"
#include "Position.hpp"
#include "ProtocolConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "ShipSpawnAnnouncer.hpp"
#include "ShipStartPosition.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {
namespace {

using testing::FIVE_PLAYER_PORTS;
using testing::PLAYER_ADDRESS;
using testing::PLAYER_NICKNAME;
using testing::SESSION_TOKEN;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = FIVE_PLAYER_PORTS.at(player)};
}

/** @returns The only message of @p datagram, read as an EntitySpawned. */
network::EntitySpawned spawnedIn(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1);
  EXPECT_EQ(frames.front().type,
            static_cast<std::uint8_t>(network::EntitySpawned::TYPE));
  return network::decodeMessage<network::EntitySpawned>(frames.front().payload);
}

/** @brief An announcer whose datagrams are recorded instead of sent. */
class ShipSpawnAnnouncerTest : public ::testing::Test {
 protected:
  /** @brief Connects the player number @p index + 1 and announces it. */
  ConnectedPlayer& join(std::size_t index) {
    ConnectedPlayer* const player = roster_.add(
        playerEndpoint(index), std::string{PLAYER_NICKNAME}, SESSION_TOKEN);
    announcer_.onPlayerJoined(*player);
    return *player;
  }

  /** @returns The datagrams sent to @p index, oldest first. */
  [[nodiscard]] std::vector<network::OutgoingDatagram> sentTo(
      std::size_t index) const {
    std::vector<network::OutgoingDatagram> datagrams;
    for (const network::OutgoingDatagram& datagram : sent_) {
      if (datagram.destination == playerEndpoint(index)) {
        datagrams.push_back(datagram);
      }
    }
    return datagrams;
  }

  [[nodiscard]] game::World& world() { return world_; }
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const {
    return sent_;
  }

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
  ShipSpawnAnnouncer announcer_{world_, roster_,
                                [this](network::OutgoingDatagram datagram) {
                                  sent_.push_back(std::move(datagram));
                                }};
};

/**
 * Given an empty game
 * When the first player joins
 * Then its ship is in the world at the start position of player 1 and the
 * player is sent that one EntitySpawned
 */
TEST_F(ShipSpawnAnnouncerTest, TheFirstPlayerIsToldAboutItsOwnShip) {
  const ConnectedPlayer& player = join(0);

  EXPECT_EQ(world().entityCount(), 1U);
  EXPECT_NE(player.shipEntityId, identifiers::NO_ENTITY_ID);
  ASSERT_EQ(sent().size(), 1U);
  const network::EntitySpawned spawned = spawnedIn(sent().front());
  const game::Position expected =
      game::shipStartPosition(1, network::MAXIMUM_PLAYERS);
  EXPECT_EQ(spawned.entityId, player.shipEntityId);
  EXPECT_EQ(spawned.entityType, network::WireEntityType::PlayerShip);
  EXPECT_EQ(spawned.x, expected.x);
  EXPECT_EQ(spawned.y, expected.y);
}

/**
 * Given player 1 already in the game
 * When player 2 joins
 * Then player 1 is sent the ship of player 2 at the start position of
 * player 2, and player 2 is sent its own ship and the ship of player 1
 */
TEST_F(ShipSpawnAnnouncerTest, TheOthersAreToldAboutANewShipAndItSeesTheirs) {
  const ConnectedPlayer& first = join(0);
  const ConnectedPlayer& second = join(1);

  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), 2U);
  const network::EntitySpawned secondShip = spawnedIn(toFirst.back());
  const game::Position expected =
      game::shipStartPosition(2, network::MAXIMUM_PLAYERS);
  EXPECT_EQ(secondShip.entityId, second.shipEntityId);
  EXPECT_EQ(secondShip.x, expected.x);
  EXPECT_EQ(secondShip.y, expected.y);
  const std::vector<network::OutgoingDatagram> toSecond = sentTo(1);
  ASSERT_EQ(toSecond.size(), 2U);
  const std::set<std::uint32_t> shipsSeenBySecond{
      spawnedIn(toSecond.front()).entityId,
      spawnedIn(toSecond.back()).entityId};
  EXPECT_EQ(shipsSeenBySecond,
            (std::set<std::uint32_t>{first.shipEntityId, second.shipEntityId}));
  EXPECT_NE(first.shipEntityId, second.shipEntityId);
}

/**
 * Given four players in the game
 * When the ships of players 1 to 4 are compared
 * Then no two ships share a position
 */
TEST_F(ShipSpawnAnnouncerTest, FourPlayersGetFourDistinctShips) {
  for (std::size_t index = 0; index < network::MAXIMUM_PLAYERS; ++index) {
    join(index);
  }

  EXPECT_EQ(world().entityCount(), network::MAXIMUM_PLAYERS);
  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), network::MAXIMUM_PLAYERS);
  for (std::size_t first = 0; first < toFirst.size(); ++first) {
    for (std::size_t second = first + 1; second < toFirst.size(); ++second) {
      EXPECT_NE(spawnedIn(toFirst.at(first)).y,
                spawnedIn(toFirst.at(second)).y);
    }
  }
}

/**
 * Given a player who already has a ship
 * When the announcer is told again that it joined
 * Then no second ship is created and nothing is sent
 */
TEST_F(ShipSpawnAnnouncerTest, APlayerWithAShipGetsNoSecondOne) {
  const ConnectedPlayer& player = join(0);
  const std::size_t sentBefore = sent().size();
  const std::uint32_t shipBefore = player.shipEntityId;

  // join() adds the same endpoint again: the roster returns the same player.
  join(0);

  EXPECT_EQ(world().entityCount(), 1U);
  EXPECT_EQ(player.shipEntityId, shipBefore);
  EXPECT_EQ(sent().size(), sentBefore);
}

/**
 * Given player 1 who was sent one datagram
 * When player 2 joins and player 1 is sent a second one
 * Then the sequence of the datagrams to player 1 goes 0, 1 and carries its
 * session token
 */
TEST_F(ShipSpawnAnnouncerTest, DatagramsCarryTheSessionAndAdvanceTheSequence) {
  join(0);
  join(1);

  const std::vector<network::OutgoingDatagram> toFirst = sentTo(0);
  ASSERT_EQ(toFirst.size(), 2U);
  const network::PacketHeader first =
      network::DatagramCodec::decodeHeader(toFirst.front().payload);
  const network::PacketHeader second =
      network::DatagramCodec::decodeHeader(toFirst.back().payload);
  EXPECT_EQ(first.sessionToken, SESSION_TOKEN);
  EXPECT_EQ(first.sequence, 0);
  EXPECT_EQ(second.sequence, 1);
}

}  // namespace
}  // namespace rtype::server
