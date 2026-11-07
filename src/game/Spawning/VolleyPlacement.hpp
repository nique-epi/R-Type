#pragma once

#include <cstddef>
#include <optional>
#include "BodyPlacement.hpp"
#include "Position.hpp"
#include "VolleySheet.hpp"

namespace rtype::game {

/**
 * @returns Where body @p index of @p volley, fired by an enemy whose center
 * is @p center, appears and steers from, as VolleySheet lays it; nothing for
 * a body of its gap.
 */
[[nodiscard]] std::optional<BodyPlacement> volleyPlacement(
    const VolleySheet& volley, Position center, std::size_t index);

}  // namespace rtype::game
