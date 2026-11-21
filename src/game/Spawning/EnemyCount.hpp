#pragma once

#include <cstddef>
#include "ComponentRegistry.hpp"

namespace rtype::game {

/** @returns How many entities of @p components are of side Enemy: the Bydos
 * of the waves and of the volleys, and the bosses. */
[[nodiscard]] std::size_t enemiesIn(engine::ComponentRegistry& components);

/** @returns How many entities of @p components are bosses: those that hold a
 * Boss, from their warning to their death. */
[[nodiscard]] std::size_t bossesIn(engine::ComponentRegistry& components);

}  // namespace rtype::game
