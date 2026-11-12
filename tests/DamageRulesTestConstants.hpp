#pragma once

#include <cstdint>
#include <string_view>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "MissileOwner.hpp"
#include "WorldTestConstants.hpp"

namespace rtype::game::testing {

/** @brief The network ids of the missiles, the Bydos and the ship of the
 * damage tests. */
constexpr std::uint32_t MISSILE_ID = 7;
constexpr std::uint32_t OTHER_MISSILE_ID = 8;
constexpr std::uint32_t BYDO_ID = 21;
constexpr std::uint32_t OTHER_BYDO_ID = 22;
constexpr std::uint32_t SHIP_ID = 31;
constexpr std::uint32_t ENEMY_MISSILE_ID = 41;

/** @brief The health of a Bydo that takes one missile; one that takes three
 * has THREE_HEALTH_POINTS of the world tests. */
constexpr int ONE_HEALTH_POINT = 1;

/** @brief The players whose missiles hit, the second one numbered
 * SECOND_PLAYER_NUMBER of the world tests. */
constexpr MissileOwner FIRST_PLAYER{.playerNumber = 1};
constexpr MissileOwner SECOND_PLAYER{.playerNumber = SECOND_PLAYER_NUMBER};

/** @brief A Bydo of five health points, a missile whose sheet makes it take
 * three, and the two points the rule leaves the Bydo. */
constexpr int FIVE_HEALTH_POINTS = 5;
constexpr int DAMAGE_OF_THREE = 3;
constexpr int TWO_HEALTH_POINTS_LEFT = 2;

/** @brief The name of the handler the reaction tests tell of the hits, a Bydo
 * of two points that a missile of three destroys, and the wall of steel they
 * hit. */
constexpr std::string_view CRACK_HANDLER = "crack";
constexpr int TWO_HEALTH_POINTS = 2;
constexpr int NO_POINT_LEFT = 0;
constexpr std::uint32_t WALL_ID = 41;

/** @brief What destroying a Bydo of the tests is worth. */
constexpr std::uint32_t BYDO_POINTS = 200;

/** @brief A Bydo with four health points that is fragile in the recover of
 * its attack 0, as the Drillback is while stunned. */
constexpr int FOUR_HEALTH_POINTS = 4;
constexpr ActionKey STUNNED{.attack = 0, .phase = EnemyPhase::Recover};
constexpr ActionKey CHARGING{.attack = 0, .phase = EnemyPhase::Strike};

}  // namespace rtype::game::testing
