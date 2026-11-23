#pragma once

#include <cstddef>
#include "BodyPlacement.hpp"
#include "ShotPattern.hpp"
#include "ShotStart.hpp"

namespace rtype::game {

/**
 * @returns Where projectile @p index of a shot of @p pattern fired from
 * @p start appears and steers from, laid as a volley of the same shape (see
 * volleyPlacement()): its count, its spread, its step on the vertical axis,
 * centred on the start, and its distance, turned to the right for a ship of
 * a player and to the left for any other side. One projectile with no
 * distance appears and steers from the start itself.
 */
[[nodiscard]] BodyPlacement shotPlacement(const ShotPattern& pattern,
                                          ShotStart start, std::size_t index);

}  // namespace rtype::game
