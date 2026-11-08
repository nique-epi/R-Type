#pragma once

#include <chrono>
#include <cstdint>
#include "BossStatus.hpp"
#include "EnemyAction.hpp"
#include "TimeConstants.hpp"

/** @brief The boss of the presentation tests: entity 51, its stage starting,
 * then its fight with 240 points; its roar, the strike of its attack 0. */
constexpr std::uint32_t PRESENTED_BOSS_ID = 51;
constexpr rtype::network::BossStatus PRESENTED_BOSS_ARRIVING{
    .stage = rtype::network::BossStage::Arriving};
constexpr rtype::network::BossStatus PRESENTED_BOSS_FIGHTING{
    .bossEntityId = PRESENTED_BOSS_ID,
    .health = 240,
    .fullHealth = 240,
    .stage = rtype::network::BossStage::Fighting};
constexpr rtype::network::EnemyAction PRESENTED_BOSS_ROAR{
    .entityId = PRESENTED_BOSS_ID,
    .attack = 0,
    .phase = rtype::network::EnemyActionPhase::Strike};

/** @brief The time of the first step of the boss walking in, and a time it
 * roars at. */
constexpr rtype::engine::Duration TO_THE_FIRST_STEP =
    std::chrono::milliseconds(4400);
constexpr rtype::engine::Duration TO_THE_ROAR = std::chrono::milliseconds(6300);
