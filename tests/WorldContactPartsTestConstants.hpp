#pragma once

#include <cstdint>
#include "ActionKey.hpp"
#include "EnemyPhase.hpp"
#include "HurtBox.hpp"
#include "HurtZone.hpp"

namespace rtype::game::testing {

/** @brief The hurt zone the contact parts tests give a sheet: a box that hurts
 * all the time and a box of the strike of attack 3. */
inline const HurtZone SOME_HURT_ZONE{
    .boxes = {HurtBox{.offsetX = 48.0F,
                      .offsetY = 0.0F,
                      .width = 64.0F,
                      .height = 32.0F,
                      .activeIn = std::nullopt},
              HurtBox{.offsetX = 0.0F,
                      .offsetY = 16.0F,
                      .width = 40.0F,
                      .height = 24.0F,
                      .activeIn = ActionKey{.attack = 3,
                                            .phase = EnemyPhase::Strike}}}};

}  // namespace rtype::game::testing
