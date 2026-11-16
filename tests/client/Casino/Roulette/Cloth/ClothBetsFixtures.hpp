#pragma once

#include <vector>
#include "ClothPlace.hpp"
#include "RouletteBet.hpp"

namespace rtype::client::testing {

/** @returns Every place of the cloth the arrows reach from the place the
 * table opens on, each once. */
[[nodiscard]] std::vector<ClothPlace> reachableClothPlaces();

/**
 * @returns Whether the server takes @p bet as lying on the cloth, as the
 * rules of RouletteRules.hpp state them: a straight on 0 to 36; a split on a
 * number with a next one in its street; a split across, a street, a corner
 * or a six line that stays on the cloth and opens its street where it must;
 * a dozen on 1, 13 or 25, a column on 1, 2 or 3; an even chance on 0.
 */
[[nodiscard]] bool serverTakes(const network::RouletteBet& bet);

}  // namespace rtype::client::testing
