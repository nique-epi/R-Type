#pragma once

#include <chrono>
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief How long the client takes a second EnemyAction with the same (enemy,
 * attack, phase) for a copy of the first: a behaviour never enters the same
 * phase of the same attack of one enemy twice within this time (see the
 * Guaranteed messages section of the protocol page).
 */
constexpr engine::Duration ENEMY_ACTION_REPEAT_WINDOW = std::chrono::seconds(2);

}  // namespace rtype::game
