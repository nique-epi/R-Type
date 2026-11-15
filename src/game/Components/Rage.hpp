#pragma once

namespace rtype::game {

/** @brief Marker of an enemy that enraged, and stays so: its behaviour sees
 * it (see BehaviourView::enraged). A boss gets it once its health falls
 * below its rage threshold (see BossPhaseSystem). */
struct Rage {};

}  // namespace rtype::game
