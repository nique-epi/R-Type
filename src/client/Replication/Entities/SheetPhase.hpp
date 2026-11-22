#pragma once

#include "EnemyAction.hpp"
#include "EnemyPhase.hpp"

namespace rtype::client {

/** @returns The phase of the sheets that the phase of the wire @p phase
 * names. */
[[nodiscard]] game::EnemyPhase sheetPhaseOf(network::EnemyActionPhase phase);

}  // namespace rtype::client
