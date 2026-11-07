#pragma once

#include "EnemyActionStart.hpp"
#include "Entity.hpp"

namespace rtype::engine {

class ComponentRegistry;
class EventBus;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @brief Records that @p start.enemy is in @p start.action (its CurrentAction),
 * from now, and publishes an EnemyActionStarted on @p events, which the server
 * sends to the players as a guaranteed EnemyAction.
 *
 * Does nothing for an entity with no CurrentAction or no NetworkIdentity. Safe
 * inside a forEach of @p components. The client handles each EnemyAction once,
 * by its number on the ordered channel, so an enemy may enter the same
 * (attack, phase) again at any time (see the Ordered messages section of the
 * protocol page).
 */
void startEnemyAction(engine::EventBus& events,
                      engine::ComponentRegistry& components,
                      const EnemyActionStart& start);

/**
 * @brief Empties the CurrentAction of @p enemy: it is idle again, from now,
 * and counts one more ended action.
 * Nothing is published; the client returns to the idle animation by itself when
 * the last animation of an action ends.
 */
void endEnemyAction(engine::ComponentRegistry& components,
                    engine::Entity enemy);

}  // namespace rtype::game
