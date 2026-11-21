#pragma once

#include <chrono>
#include <cstdint>
#include <string_view>
#include "BossStatus.hpp"
#include "EnemyAction.hpp"
#include "EntityDestroyed.hpp"
#include "TimeConstants.hpp"

/** @brief The boss of the presentation tests: entity 51, its stage starting,
 * then its fight with 240 points, then its death, its removal by a restart
 * and its body killed; its roar, the strike of its attack 0. */
constexpr std::uint32_t PRESENTED_BOSS_ID = 51;

/** @brief The name the sheet of the boss gives its bar. */
constexpr std::string_view PRESENTED_BOSS_NAME = "NECROMANCER";

/** @brief One update of the client, a frame at 60 frames a second. */
constexpr rtype::engine::Duration ONE_UPDATE = std::chrono::milliseconds(16);
constexpr rtype::network::BossStatus PRESENTED_BOSS_ARRIVING{
    .stage = rtype::network::BossStage::Arriving};
constexpr rtype::network::BossStatus PRESENTED_BOSS_FIGHTING{
    .bossEntityId = PRESENTED_BOSS_ID,
    .health = 240,
    .fullHealth = 240,
    .stage = rtype::network::BossStage::Fighting};
constexpr rtype::network::BossStatus PRESENTED_BOSS_DEFEATED{
    .bossEntityId = PRESENTED_BOSS_ID,
    .health = 0,
    .fullHealth = 240,
    .stage = rtype::network::BossStage::Defeated};
constexpr rtype::network::EntityDestroyed PRESENTED_BOSS_REMOVED{
    .entityId = PRESENTED_BOSS_ID,
    .reason = rtype::network::DestructionReason::Removed};
constexpr rtype::network::EntityDestroyed PRESENTED_BOSS_KILLED{
    .entityId = PRESENTED_BOSS_ID,
    .reason = rtype::network::DestructionReason::Killed};
constexpr rtype::network::EnemyAction PRESENTED_BOSS_ROAR{
    .entityId = PRESENTED_BOSS_ID,
    .attack = 0,
    .phase = rtype::network::EnemyActionPhase::Strike};

/** @brief The time of the first step of the boss walking in, and a time it
 * roars at. */
constexpr rtype::engine::Duration TO_THE_FIRST_STEP =
    std::chrono::milliseconds(4400);
constexpr rtype::engine::Duration TO_THE_ROAR = std::chrono::milliseconds(6300);

/** @brief The music a level names, which plays through its bosses. */
constexpr std::string_view LEVEL_MUSIC = "music/crawl_out.mp3";
/** @brief The music the sheet of a boss of its own music names. */
constexpr std::string_view PRESENTED_BOSS_MUSIC =
    "music/freight_train_boogie.mp3";
