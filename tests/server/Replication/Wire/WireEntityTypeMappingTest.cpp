#include <gtest/gtest.h>
#include <array>
#include <optional>
#include <utility>
#include "EnemyKind.hpp"
#include "EntityType.hpp"
#include "WireEntityTypeMapping.hpp"
#include "WorldState.hpp"

using rtype::game::EnemyKind;
using rtype::game::EntityType;
using rtype::network::WireEntityType;
using rtype::server::toWireEntityType;

/**
 * Given each kind of enemy
 * When the wire type of an Enemy of that kind is asked for
 * Then the Drifter and the Weaver are a Bydo and each other kind has the type
 * the protocol page reserves for it
 */
TEST(WireEntityTypeMapping, EachKindOfEnemyHasItsWireType) {
  const auto expected = std::to_array<std::pair<EnemyKind, WireEntityType>>(
      {{EnemyKind::Drifter, WireEntityType::Bydo},
       {EnemyKind::Weaver, WireEntityType::Bydo},
       {EnemyKind::Voltjelly, WireEntityType::Voltjelly},
       {EnemyKind::Reaper, WireEntityType::Reaper},
       {EnemyKind::Bydomancer, WireEntityType::Bydomancer},
       {EnemyKind::Drillback, WireEntityType::Drillback},
       {EnemyKind::BydoTyrant, WireEntityType::BydoTyrant}});

  for (const auto& [kind, wireType] : expected) {
    SCOPED_TRACE(static_cast<int>(kind));

    EXPECT_EQ(toWireEntityType(EntityType::Enemy, kind), wireType);
  }
}

/**
 * Given an Enemy that has no kind
 * When its wire type is asked for
 * Then it is a Bydo
 */
TEST(WireEntityTypeMapping, AnEnemyWithoutAKindIsABydo) {
  EXPECT_EQ(toWireEntityType(EntityType::Enemy, std::nullopt),
            WireEntityType::Bydo);
}

/**
 * Given a ship and the two missiles, whatever kind is passed with them
 * When their wire types are asked for
 * Then the kind is ignored and each has its own type
 */
TEST(WireEntityTypeMapping, AKindChangesNothingForTheOtherTypes) {
  EXPECT_EQ(toWireEntityType(EntityType::Player, EnemyKind::Reaper),
            WireEntityType::PlayerShip);
  EXPECT_EQ(toWireEntityType(EntityType::PlayerMissile, EnemyKind::Reaper),
            WireEntityType::PlayerMissile);
  EXPECT_EQ(toWireEntityType(EntityType::EnemyMissile, EnemyKind::Reaper),
            WireEntityType::EnemyMissile);
}
