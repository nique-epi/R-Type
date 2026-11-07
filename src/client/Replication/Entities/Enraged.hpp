#pragma once

namespace rtype::client {

/**
 * @brief Component of an enemy the server announced enraged, by an
 * EnemyAction of phase Enraged: from then on the idle animation it goes back
 * to is the enraged idle of its sheet (see enragedIdleAnimationOf()). It
 * stays until the entity is destroyed.
 */
struct Enraged {};

}  // namespace rtype::client
