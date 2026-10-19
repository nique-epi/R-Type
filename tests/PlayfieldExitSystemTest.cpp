#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include "ComponentRegistry.hpp"
#include "Entity.hpp"
#include "EntityDestruction.hpp"
#include "EntityRegistry.hpp"
#include "EntityType.hpp"
#include "EventBus.hpp"
#include "NetworkIdentity.hpp"
#include "PlayfieldExitSystem.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

using rtype::engine::ComponentRegistry;
using rtype::engine::Entity;
using rtype::engine::EntityRegistry;
using rtype::engine::EventBus;
using rtype::engine::SIMULATION_TICK_DURATION;
using rtype::game::DestructionCause;
using rtype::game::EntityDestruction;
using rtype::game::EntityType;
using rtype::game::NetworkIdentity;
using rtype::game::PlayfieldExitSystem;
using rtype::game::Position;

namespace {

constexpr float SOME_Y = 300.0F;
constexpr float JUST_PAST_THE_MARGIN_X = -65.0F;
constexpr float EXACTLY_AT_THE_MARGIN_X = -64.0F;
constexpr float FAR_PAST_THE_EDGE_X = -1000.0F;
constexpr std::uint32_t SOME_NETWORK_ID = 7;

class PlayfieldExitSystemTest : public ::testing::Test {
 protected:
  PlayfieldExitSystemTest() {
    events_.subscribe<EntityDestruction>(
        [this](const EntityDestruction& event) { received_.push_back(event); });
  }

  Entity add(EntityType type, float positionX) {
    const Entity entity = entities_.create();
    components_.add(entity, Position{.x = positionX, .y = SOME_Y});
    components_.add(entity, type);
    components_.add(entity, NetworkIdentity{.networkId = SOME_NETWORK_ID});
    return entity;
  }

  void updateAndDispatch() {
    exit_.update(components_, SIMULATION_TICK_DURATION);
    events_.dispatch();
  }

  [[nodiscard]] const EntityRegistry& entities() const { return entities_; }
  [[nodiscard]] const std::vector<EntityDestruction>& received() const {
    return received_;
  }

 private:
  EntityRegistry entities_;
  ComponentRegistry components_{entities_};
  EventBus events_;
  PlayfieldExitSystem exit_{events_};
  std::vector<EntityDestruction> received_;
};

}  // namespace

/**
 * Given an enemy 65 units past the left edge
 * When the system runs
 * Then the enemy is gone and its removal was announced under its network id
 */
TEST_F(PlayfieldExitSystemTest, RemovesAnEnemyPastTheLeftEdge) {
  const Entity enemy = add(EntityType::Enemy, JUST_PAST_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_FALSE(entities().isAlive(enemy));
  ASSERT_EQ(received().size(), 1U);
  EXPECT_EQ(received().front(),
            (EntityDestruction{.networkId = SOME_NETWORK_ID,
                               .cause = DestructionCause::Removed}));
}

/**
 * Given an enemy exactly 64 units past the left edge
 * When the system runs
 * Then it is still in the world and nothing was announced
 */
TEST_F(PlayfieldExitSystemTest, KeepsAnEnemyExactlyAtTheMargin) {
  const Entity enemy = add(EntityType::Enemy, EXACTLY_AT_THE_MARGIN_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(enemy));
  EXPECT_TRUE(received().empty());
}

/**
 * Given a ship far past the left edge
 * When the system runs
 * Then the ship is still in the world
 */
TEST_F(PlayfieldExitSystemTest, NeverRemovesAShip) {
  const Entity ship = add(EntityType::Player, FAR_PAST_THE_EDGE_X);

  updateAndDispatch();

  EXPECT_TRUE(entities().isAlive(ship));
  EXPECT_TRUE(received().empty());
}

/**
 * Given an enemy that was removed by a first run
 * When the system runs again
 * Then the removal was announced once, not twice
 */
TEST_F(PlayfieldExitSystemTest, AnnouncesARemovalOnlyOnce) {
  static_cast<void>(add(EntityType::Enemy, JUST_PAST_THE_MARGIN_X));
  updateAndDispatch();

  updateAndDispatch();

  EXPECT_EQ(received().size(), 1U);
}
