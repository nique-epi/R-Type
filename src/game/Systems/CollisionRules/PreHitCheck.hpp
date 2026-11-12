#pragma once

#include "MissileHit.hpp"

namespace rtype::engine {

class ComponentRegistry;

}  // namespace rtype::engine

namespace rtype::game {

/**
 * @returns Whether @p hit may be announced: true unless the target is a body
 * a behaviour pilots, whose behaviour state cancels it (see
 * IBehaviourState::letsHit()). The state is told the network id of the
 * missile and where the missile lies from the target, read in @p components
 * now: a missile tested where its player saw the target is told where it is,
 * not where it was tested. A body piloted by a behaviour that keeps no state,
 * or not yet steered, takes every hit, and asking it costs one lookup.
 */
[[nodiscard]] bool targetLetsHit(engine::ComponentRegistry& components,
                                 const MissileHit& hit);

}  // namespace rtype::game
