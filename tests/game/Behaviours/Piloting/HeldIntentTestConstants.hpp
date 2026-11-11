#pragma once

#include <cstdint>
#include <string_view>
#include "ActionKey.hpp"
#include "Direction.hpp"
#include "EnemyPhase.hpp"
#include "Intent.hpp"

/** @brief A step that goes left, and one that also starts the strike of
 * attack 1. */
constexpr rtype::game::Intent GOING_LEFT{
    .direction = rtype::game::Direction{.x = -1.0F, .y = 0.0F},
    .steered = true};
constexpr rtype::game::Intent STRIKING{
    .steered = true,
    .startsAction = rtype::game::ActionKey{
        .attack = 1, .phase = rtype::game::EnemyPhase::Strike}};

/** @brief An effect a step plays. */
constexpr std::string_view SPARK_EFFECT = "spark";

/** @brief A wait of three ticks: the tick of the step and two more. */
constexpr std::uint32_t THREE_TICKS = 3;
constexpr std::uint32_t TICKS_HELD_AFTER_THE_STEP = 2;
constexpr std::uint32_t NO_WAIT = 0;
