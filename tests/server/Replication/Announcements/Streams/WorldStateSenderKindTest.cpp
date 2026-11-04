#include <gtest/gtest.h>
#include <vector>
#include "EnemyKind.hpp"
#include "WorldState.hpp"
#include "WorldStateSenderFixture.hpp"
#include "WorldStateSenderKindFixture.hpp"
#include "WorldStateSenderTestConstants.hpp"

using rtype::game::EnemyKind;
using rtype::network::EntityState;
using rtype::network::WireEntityType;
using rtype::server::testing::ENEMY_NETWORK_ID;
using rtype::server::testing::ENEMY_POSITION;
using rtype::server::testing::expectedEntity;
using rtype::server::testing::WorldStateSenderKindFixture;

/**
 * Given a Reaper in the world and a player
 * When the first state is due
 * Then the Reaper is listed under the wire type of the Reaper, not as a Bydo
 */
TEST_F(WorldStateSenderKindFixture, ListsAnEnemyUnderTheWireTypeOfItsKind) {
  addEnemy(EnemyKind::Reaper, ENEMY_NETWORK_ID);

  const std::vector<EntityState> listed = listedEntities();

  EXPECT_EQ(listed,
            (std::vector<EntityState>{expectedEntity(
                ENEMY_NETWORK_ID, WireEntityType::Reaper, ENEMY_POSITION)}));
}

/**
 * Given a Weaver in the world and a player
 * When the first state is due
 * Then the Weaver is listed as a Bydo, the type it shares with the Drifter
 */
TEST_F(WorldStateSenderKindFixture, ListsAWeaverAsABydo) {
  addEnemy(EnemyKind::Weaver, ENEMY_NETWORK_ID);

  const std::vector<EntityState> listed = listedEntities();

  EXPECT_EQ(listed,
            (std::vector<EntityState>{expectedEntity(
                ENEMY_NETWORK_ID, WireEntityType::Bydo, ENEMY_POSITION)}));
}
