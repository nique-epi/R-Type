#include <gtest/gtest.h>
#include "EnemyKind.hpp"
#include "MovementPattern.hpp"

using rtype::game::ALL_ENEMY_KINDS;
using rtype::game::EnemyKind;
using rtype::game::PatternShape;

namespace {

constexpr std::uint32_t DRIFTER_POINTS_ON_THE_PAGE = 100;
constexpr std::uint32_t WEAVER_POINTS_ON_THE_PAGE = 200;

}  // namespace

/**
 * Given every kind of enemy
 * When its pattern and its points are asked for
 * Then the Weaver waves for 200 points and every other kind goes straight for
 * 100, the Drifter's, until its own pull request
 */
TEST(EnemyKind, OnlyTheWeaverDiffersFromTheDrifterForNow) {
  for (const EnemyKind kind : ALL_ENEMY_KINDS) {
    SCOPED_TRACE(static_cast<int>(kind));
    const bool isWeaver = kind == EnemyKind::Weaver;

    EXPECT_EQ(rtype::game::patternShapeOf(kind),
              isWeaver ? PatternShape::Wave : PatternShape::Straight);
    EXPECT_EQ(rtype::game::pointsOf(kind), isWeaver
                                               ? WEAVER_POINTS_ON_THE_PAGE
                                               : DRIFTER_POINTS_ON_THE_PAGE);
  }
}
