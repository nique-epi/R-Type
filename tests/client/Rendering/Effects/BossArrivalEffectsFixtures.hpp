#pragma once

#include <vector>
#include "ComponentRegistry.hpp"
#include "Position.hpp"

/** @returns Where every shockwave of @p components opens from. */
[[nodiscard]] std::vector<rtype::game::Position> shockwaveCentersOf(
    rtype::engine::ComponentRegistry& components);
