#pragma once

#include <cstdint>
#include "DamageScene.hpp"

namespace rtype::game::testing {

/** @returns The announcement that the entity @p networkId was removed. */
[[nodiscard]] Announcement removed(std::uint32_t networkId);

/** @returns The announcement that the entity @p networkId was killed. */
[[nodiscard]] Announcement killed(std::uint32_t networkId);

}  // namespace rtype::game::testing
