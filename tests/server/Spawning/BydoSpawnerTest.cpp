#include <gtest/gtest.h>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>
#include "BydoSpawner.hpp"
#include "BydoSpawnerTestConstants.hpp"
#include "DatagramCodec.hpp"
#include "Endpoint.hpp"
#include "Entity.hpp"
#include "EntitySpawned.hpp"
#include "EntityType.hpp"
#include "JournalFile.hpp"
#include "Logger.hpp"
#include "MessageFrame.hpp"
#include "MessagePayload.hpp"
#include "MovementPattern.hpp"
#include "OutgoingDatagram.hpp"
#include "PlayerRoster.hpp"
#include "PlayfieldConstants.hpp"
#include "ServerLobbyTestConstants.hpp"
#include "SpawningConstants.hpp"
#include "World.hpp"
#include "WorldState.hpp"

namespace rtype::server {
namespace {

using testing::PLAYER_ADDRESS;
using testing::PLAYER_NICKNAME;
using testing::SESSION_TOKEN;

constexpr std::uint32_t SOME_SEED = 42;
constexpr std::uint32_t OTHER_SEED = 43;
constexpr std::size_t SOME_INTERVALS = 20;
constexpr std::size_t FIRST_PLAYER = 0;
constexpr std::size_t SECOND_PLAYER = 1;

network::Endpoint playerEndpoint(std::size_t player) {
  return network::Endpoint{.address = std::string{PLAYER_ADDRESS},
                           .port = static_cast<std::uint16_t>(
                               testing::FIVE_PLAYER_PORTS.at(player))};
}

/** @returns The only message of @p datagram, read as an EntitySpawned. */
network::EntitySpawned spawnedIn(const network::OutgoingDatagram& datagram) {
  const std::vector<network::MessageFrame> frames =
      network::DatagramCodec::splitMessages(datagram.payload);
  EXPECT_EQ(frames.size(), 1U);
  EXPECT_EQ(frames.front().type,
            static_cast<std::uint8_t>(network::EntitySpawned::TYPE));
  return network::decodeMessage<network::EntitySpawned>(frames.front().payload);
}

/** @brief A world and a roster the spawner works on, its datagrams recorded. */
class Rig {
 public:
  void connect(std::size_t index) {
    roster_.add(playerEndpoint(index), std::string{PLAYER_NICKNAME},
                SESSION_TOKEN);
  }

  /** @brief A spawner on this world, sending into sent(). */
  [[nodiscard]] BydoSpawner makeSpawner(std::uint32_t seed) {
    return BydoSpawner{world_, roster_,
                       [this](network::OutgoingDatagram datagram) {
                         sent_.push_back(std::move(datagram));
                       },
                       seed};
  }

  /** @returns The heights of the Bydos announced to @p index, oldest first. */
  [[nodiscard]] std::vector<float> heightsSentTo(std::size_t index) const {
    std::vector<float> heights;
    for (const network::OutgoingDatagram& datagram : sent_) {
      if (datagram.destination == playerEndpoint(index)) {
        heights.push_back(spawnedIn(datagram).y);
      }
    }
    return heights;
  }

  static void tick(BydoSpawner& spawner, std::size_t count) {
    for (std::size_t tickNumber = 0; tickNumber < count; ++tickNumber) {
      spawner.onTick();
    }
  }

  [[nodiscard]] game::World& world() { return world_; }
  [[nodiscard]] const std::vector<network::OutgoingDatagram>& sent() const {
    return sent_;
  }

 private:
  game::World world_;
  PlayerRoster roster_;
  std::vector<network::OutgoingDatagram> sent_;
};

class BydoSpawnerTest : public ::testing::Test, public Rig {};

/**
 * Given a spawner and a player
 * When one tick less than the interval has passed
 * Then no Bydo has appeared and nothing was sent
 */
TEST_F(BydoSpawnerTest, NothingAppearsBeforeTheInterval) {
  connect(FIRST_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);

  tick(spawner, testing::TICKS_PER_BYDO_INTERVAL - 1);

  EXPECT_EQ(world().entityCount(), 0U);
  EXPECT_TRUE(sent().empty());
}

/**
 * Given a spawner and a player
 * When the interval has just passed
 * Then one Bydo is in the world and the player is sent one EntitySpawned for
 * it, past the right edge
 */
TEST_F(BydoSpawnerTest, ABydoAppearsOffScreenWhenTheIntervalPasses) {
  connect(FIRST_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);

  tick(spawner, testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(world().entityCount(), 1U);
  ASSERT_EQ(sent().size(), 1U);
  const network::EntitySpawned spawned = spawnedIn(sent().front());
  EXPECT_EQ(spawned.entityType, network::WireEntityType::Bydo);
  EXPECT_GT(spawned.x, game::PLAYFIELD_WIDTH);
  EXPECT_GE(spawned.y, game::BYDO_SPAWN_VERTICAL_MARGIN);
  EXPECT_LE(spawned.y,
            game::PLAYFIELD_HEIGHT - game::BYDO_SPAWN_VERTICAL_MARGIN);
}

/**
 * Given a spawner and two players
 * When two intervals have passed
 * Then two Bydos are in the world and each player was sent both, at the same
 * heights
 */
TEST_F(BydoSpawnerTest, EveryPlayerIsToldAboutEveryBydo) {
  connect(FIRST_PLAYER);
  connect(SECOND_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);

  tick(spawner, 2 * testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(world().entityCount(), 2U);
  const std::vector<float> first = heightsSentTo(FIRST_PLAYER);
  EXPECT_EQ(first.size(), 2U);
  EXPECT_EQ(first, heightsSentTo(SECOND_PLAYER));
}

/**
 * Given two spawners started from the same seed
 * When the same intervals pass for both
 * Then they announce the same heights in the same order, and a spawner started
 * from another seed announces other heights
 */
TEST_F(BydoSpawnerTest, TheSeedDecidesTheHeights) {
  connect(FIRST_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);
  tick(spawner, SOME_INTERVALS * testing::TICKS_PER_BYDO_INTERVAL);
  const std::vector<float> reference = heightsSentTo(FIRST_PLAYER);
  ASSERT_EQ(reference.size(), SOME_INTERVALS);

  Rig replayed;
  replayed.connect(FIRST_PLAYER);
  BydoSpawner sameSeed = replayed.makeSpawner(SOME_SEED);
  tick(sameSeed, SOME_INTERVALS * testing::TICKS_PER_BYDO_INTERVAL);
  Rig other;
  other.connect(FIRST_PLAYER);
  BydoSpawner otherSeed = other.makeSpawner(OTHER_SEED);
  tick(otherSeed, SOME_INTERVALS * testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(replayed.heightsSentTo(FIRST_PLAYER), reference);
  EXPECT_NE(other.heightsSentTo(FIRST_PLAYER), reference);
}

/**
 * Given twenty Bydos announced from one seed
 * When their heights are compared
 * Then they are not all the same, so the height is not a fixed row
 */
TEST_F(BydoSpawnerTest, HeightsVaryFromOneBydoToTheNext) {
  connect(FIRST_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);
  tick(spawner, SOME_INTERVALS * testing::TICKS_PER_BYDO_INTERVAL);

  const std::vector<float> heights = heightsSentTo(FIRST_PLAYER);

  EXPECT_GT(std::ranges::count_if(
                heights,
                [&heights](float height) { return height != heights.front(); }),
            0);
}

/**
 * Given no player connected
 * When an interval passes
 * Then the Bydo is in the world and nothing is sent
 */
TEST_F(BydoSpawnerTest, ABydoAppearsEvenWithNobodyToTell) {
  BydoSpawner spawner = makeSpawner(SOME_SEED);

  tick(spawner, testing::TICKS_PER_BYDO_INTERVAL);

  EXPECT_EQ(world().entityCount(), 1U);
  EXPECT_TRUE(sent().empty());
}

/**
 * Given a log file as the only sink
 * When a spawner is started from a seed
 * Then the log holds a line with that seed, so the case can be replayed
 */
TEST_F(BydoSpawnerTest, TheSeedIsLogged) {
  logging::Logger::setLevel(logging::LogLevel::Info);
  const JournalFile journal;
  logging::Logger::setOutput({.toStderr = false, .filePath = journal.path()});

  const BydoSpawner spawner = makeSpawner(SOME_SEED);

  const std::vector<std::string> lines = journal.lines();
  ASSERT_EQ(lines.size(), 1U);
  EXPECT_NE(lines.front().find(std::to_string(SOME_SEED)), std::string::npos);
}

/**
 * Given a spawner and a player
 * When three intervals have passed
 * Then two Bydos go straight and one follows a wave: the kinds alternate,
 * starting with the one that goes straight
 */
TEST_F(BydoSpawnerTest, KindsAlternateStartingWithTheOneThatGoesStraight) {
  connect(FIRST_PLAYER);
  BydoSpawner spawner = makeSpawner(SOME_SEED);

  tick(spawner, 3 * testing::TICKS_PER_BYDO_INTERVAL);

  std::size_t straight = 0;
  std::size_t waves = 0;
  world().components().forEach<game::MovementPattern, game::EntityType>(
      [&straight, &waves](engine::Entity, const game::MovementPattern& pattern,
                          const game::EntityType&) {
        straight += pattern.shape == game::PatternShape::Straight;
        waves += pattern.shape == game::PatternShape::Wave;
      });
  EXPECT_EQ(straight, 2U);
  EXPECT_EQ(waves, 1U);
}

}  // namespace
}  // namespace rtype::server
