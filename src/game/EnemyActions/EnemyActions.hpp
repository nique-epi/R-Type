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
 * inside a forEach of @p components. A behaviour must not enter the same
 * (attack, phase) of one enemy twice within two seconds: the client takes the
 * second for a copy of the first and ignores it (see the Guaranteed messages
 * section of the protocol page).
 */
void startEnemyAction(engine::EventBus& events,
                      engine::ComponentRegistry& components,
                      const EnemyActionStart& start);

/**
 * @brief Empties the CurrentAction of @p enemy: it is idle again, from now.
 * Nothing is published; the client returns to the idle animation by itself when
 * the last animation of an action ends.
 */
void endEnemyAction(engine::ComponentRegistry& components,
                    engine::Entity enemy);

}  // namespace rtype::game
