#pragma once

#include <cstdint>
#include "EntityMessage.hpp"

namespace rtype::tools::testing {

/** @returns The status of a boss that arrives: no entity, no health yet. */
[[nodiscard]] client::EntityMessage bossArriving();

/** @returns The status of a boss that fights with @p health of
 * FULL_BOSS_HEALTH points left. */
[[nodiscard]] client::EntityMessage bossFighting(std::uint16_t health);

/** @returns The status of a boss that fights enraged with @p health of
 * FULL_BOSS_HEALTH points left. */
[[nodiscard]] client::EntityMessage bossEnraged(std::uint16_t health);

/** @returns The status of a boss that was destroyed. */
[[nodiscard]] client::EntityMessage bossDefeated();

}  // namespace rtype::tools::testing
