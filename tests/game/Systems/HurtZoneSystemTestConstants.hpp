#pragma once

#include <chrono>
#include <optional>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "HurtBox.hpp"
#include "HurtWindow.hpp"
#include "TimeConstants.hpp"

namespace rtype::game::testing {

/**
 * @brief The ship of the hurt zone tests is the square of CollisionScene, 10
 * units wide. A box 10 units wide, centered 20 units to the right of the
 * enemy, hurts a ship centered within 10 units of it, edges excluded: at 20
 * units the ship is inside it, at 30 units its edge only touches the box, and
 * at 60 units it is apart.
 */
constexpr float BOX_SIDE = 10.0F;
constexpr float BOX_OFFSET_X = 20.0F;
constexpr float SHIP_INSIDE_THE_BOX_X = 20.0F;
constexpr float SHIP_TOUCHING_THE_BOX_X = 30.0F;
constexpr float SHIP_APART_FROM_THE_BOX_X = 60.0F;

/** @brief Where the enemy of the tests stands when it is moved away from the
 * origin, and where its box then stands: 100 plus BOX_OFFSET_X. */
constexpr float MOVED_ENEMY_X = 100.0F;
constexpr float SHIP_IN_THE_MOVED_BOX_X = 120.0F;

/** @brief A box that hurts all the time, and a box that hurts only in the
 * strike of attack 1. */
constexpr HurtBox ALWAYS_HURTING_BOX{.offsetX = BOX_OFFSET_X,
                                     .offsetY = 0.0F,
                                     .width = BOX_SIDE,
                                     .height = BOX_SIDE,
                                     .activeIn = std::nullopt};
constexpr ActionKey STRIKE_OF_ATTACK_ONE{.attack = 1,
                                         .phase = EnemyPhase::Strike};
constexpr HurtBox STRIKE_BOX{.offsetX = BOX_OFFSET_X,
                             .offsetY = 0.0F,
                             .width = BOX_SIDE,
                             .height = BOX_SIDE,
                             .activeIn = STRIKE_OF_ATTACK_ONE};

/** @brief Other actions than the strike of attack 1: its signal, and the
 * strike of attack 2. */
constexpr ActionKey SIGNAL_OF_ATTACK_ONE{.attack = 1,
                                         .phase = EnemyPhase::Signal};
constexpr ActionKey STRIKE_OF_ATTACK_TWO{.attack = 2,
                                         .phase = EnemyPhase::Strike};

/** @brief A box of the strike of attack 1 that hurts from 70 ms into it
 * until 140 ms, as the second frame of a 70 ms strike; one that hurts from
 * 70 ms to the end of the strike. */
constexpr engine::Duration WINDOW_START = std::chrono::milliseconds(70);
constexpr engine::Duration WINDOW_END = std::chrono::milliseconds(140);
constexpr HurtBox TIMED_STRIKE_BOX{
    .offsetX = BOX_OFFSET_X,
    .offsetY = 0.0F,
    .width = BOX_SIDE,
    .height = BOX_SIDE,
    .activeIn = STRIKE_OF_ATTACK_ONE,
    .window = HurtWindow{.from = WINDOW_START, .until = WINDOW_END}};
constexpr HurtBox OPEN_ENDED_STRIKE_BOX{
    .offsetX = BOX_OFFSET_X,
    .offsetY = 0.0F,
    .width = BOX_SIDE,
    .height = BOX_SIDE,
    .activeIn = STRIKE_OF_ATTACK_ONE,
    .window = HurtWindow{.from = WINDOW_START, .until = std::nullopt}};

/** @brief How far into a phase a test looks: one tick before the window, and
 * a second in, far past its end. */
constexpr engine::Duration JUST_BEFORE_THE_WINDOW =
    WINDOW_START - engine::SIMULATION_TICK_DURATION;
constexpr engine::Duration A_SECOND_INTO_THE_PHASE = std::chrono::seconds(1);

}  // namespace rtype::game::testing
